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
 * $brcm_Workfile: nexus_platform_pinmux.c $
 * $brcm_Revision: 23 $
 * $brcm_Date: 10/29/12 11:56a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97425/src/nexus_platform_pinmux.c $
 * 
 * 23   10/29/12 11:56a ayxiang
 * SW7425-4107: we should NOT make the DOCSIS tight with cablecard
 *  support, in fact there is not MUST relationship between them
 * 
 * 22   10/24/12 5:05p nickh
 * SW7425-4145: Fix OR'ing of pinmux fields
 * 
 * 21   10/24/12 3:50p mphillip
 * SW7425-4035: Merge 4538 support branch to main
 * 
 * SW7425-4035/1   10/4/12 5:19p mphillip
 * SW7425-4035: Initial 4538 support
 * 
 * 20   6/11/12 11:20p jessem
 * SW7425-3225: Update 97422 VMS support.
 * 
 * 19   6/11/12 4:10p nickh
 * SW7425-3219: Fix 3383 console hang on 97425VMS_SFFV20 boards
 * 
 * 18   12/8/11 1:05p mphillip
 * SW7425-1437: Merge SATIP pinmux to main
 * 
 * 17   12/2/11 6:38p jessem
 * SW7425-1896: Modify AON pinmux GPIO5 and GPIO6 to accommodate new UART
 *  config for new VMS_SFF.
 *
 * 16   9/13/11 7:00p jessem
 * SW7425-1248: Updated pinmux and I2C usage for B0.
 *
 * 15   9/6/11 12:20p hongtaoz
 * SW7425-1248: fixed compile error for B0;
 *
 * 14   8/29/11 5:28p jgarrett
 * SW7425-724: Adding external RFM control on 97425SV board
 *
 * 13   7/20/11 12:24p jessem
 * SW7425-957:Added support for 7425 DBS platform for GPIO pins 48, 49,
 *  50, 51.
 *
 * 12   4/21/11 10:41a spothana
 * SW7425-369: Correct some more MPOD pin mux programming
 *
 * 11   4/19/11 4:27p spothana
 * SW7425-369: Correct the MPOD pinx mux settings.
 *
 * 10   4/12/11 4:46p spothana
 * SW7425-331: Add MTSIF1 pin mux programming for BCM3383 on BCM97425 SFF
 *  board.
 *
 * 9   4/8/11 12:01p vishk
 * SW7425-313 : Add support for VMSSFF board with 3128 frontend.
 *
 * 8   4/1/11 4:03p nickh
 * SW7425-9: Set defaults for USB-TTL debug UARTS
 *
 * 7   3/28/11 3:07p nickh
 * SW7422-412: Add 7422 defines so we can symlink the file
 *
 * 6   3/9/11 2:13p vishk
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 *  board) plug on 7422 board  Add support for 97425 platform.
 *
 * 5   3/1/11 6:49p vishk
 * SW7422-272: Add nexus gpio support for frontend 4506 tuners
 *
 * 4   1/17/11 5:32p hongtaoz
 * SW7425-9: updated 7425 pin mux setting for tuner inputs; differentiate
 *  SV vs VMS board;
 *
 * 3   1/4/11 10:51p hongtaoz
 * SW7425-9: add tuner input support;
 *
 * 2   11/24/10 4:38p hongtaoz
 * SW7425-9: updated 7425 platform heap scheme; skip uart0 pinmux setting
 *  owned by os;
 *
 * 1   9/14/10 7:04p hongtaoz
 * SW7425-9: adding 7425 support;
 *
 * 1   9/14/10 6:53p hongtaoz
 * SW7425-9: adding 7425 support;
 *
 *
 *
 * Module Description:
*
*
* API Description:
*   This file contains pinmux initialization for the 97425 reference board.
*
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "priv/nexus_core.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#include "humax_pinmux.h"
#endif

BDBG_MODULE(nexus_platform_pinmux);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
extern const char g_humaxGpioFunctionTable[] ;
extern HUMAX_GpioTable g_humaxGpioTable[];

#include "bchp_usb_ohci.h"
#include "bchp_usb_ohci1.h"

static void  NEXUS_Platform_P_InitUSBPowerMode(void)
{
	uint32_t reg=0;
	BREG_Handle hReg = g_pCoreHandles->reg;

	/* OHCI port �� Per-Port Mode�� ���� & OCP enable ... */
	reg = BREG_Read32(hReg, BCHP_USB_OHCI_HcRhDescriptorA);
	reg &= ~(BCHP_USB_OHCI_HcRhDescriptorA_PSM_MASK | BCHP_USB_OHCI_HcRhDescriptorA_NPS_MASK | BCHP_USB_OHCI_HcRhDescriptorA_OCPM_MASK);
	reg |= ( (1<<BCHP_USB_OHCI_HcRhDescriptorA_PSM_SHIFT) | (0<<BCHP_USB_OHCI_HcRhDescriptorA_NPS_SHIFT) | (1<<BCHP_USB_OHCI_HcRhDescriptorA_OCPM_SHIFT) );
	BREG_Write32(hReg, BCHP_USB_OHCI_HcRhDescriptorA, reg);

	reg = BREG_Read32(hReg, BCHP_USB_OHCI1_HcRhDescriptorA);
	reg &= ~(BCHP_USB_OHCI_HcRhDescriptorA_PSM_MASK | BCHP_USB_OHCI_HcRhDescriptorA_NPS_MASK | BCHP_USB_OHCI_HcRhDescriptorA_OCPM_MASK);
	reg |= ( (1<<BCHP_USB_OHCI_HcRhDescriptorA_PSM_SHIFT) | (0<<BCHP_USB_OHCI_HcRhDescriptorA_NPS_SHIFT) | (1<<BCHP_USB_OHCI_HcRhDescriptorA_OCPM_SHIFT) );
	BREG_Write32(hReg, BCHP_USB_OHCI1_HcRhDescriptorA, reg);

	/* PortPowerControlMask set */
	reg = BREG_Read32(hReg, BCHP_USB_OHCI_HcRhDescriptorB);
	reg &= ~(BCHP_USB_OHCI_HcRhDescriptorB_PPCM_MASK);
	reg |= (0x0F<<BCHP_USB_OHCI_HcRhDescriptorB_PPCM_SHIFT);
	BREG_Write32(hReg, BCHP_USB_OHCI_HcRhDescriptorB, reg);

	/* PortPowerControlMask set */
	reg = BREG_Read32(hReg, BCHP_USB_OHCI1_HcRhDescriptorB);
	reg &= ~(BCHP_USB_OHCI_HcRhDescriptorB_PPCM_MASK);
	reg |= (0x0F<<BCHP_USB_OHCI_HcRhDescriptorB_PPCM_SHIFT);
	BREG_Write32(hReg, BCHP_USB_OHCI1_HcRhDescriptorB, reg);

}

NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
	BREG_Handle hReg = g_pCoreHandles->reg;
	uint32_t reg=0;
	uint32_t lastAddr = 0;
	int i;
	const char * enableMII=NEXUS_GetEnv("enable_mii");
/*	const char * enableAvdUarts=NEXUS_GetEnv("enable_avd_uarts"); */

	NEXUS_Platform_P_InitUSBPowerMode();

	for(i=0;i<HUMAX_NUM_GPIO_PINS;i++)
	{
		if(( lastAddr ==0) && (i == 0))
		{
			reg = BREG_Read32(hReg,g_humaxGpioTable[i].addr);
		}
		else if((g_humaxGpioTable[i].addr != lastAddr) && ( lastAddr !=0))
		{
			BREG_Write32 (hReg, lastAddr, reg);
			BDBG_ERR(("BREG_Write32 0x%08x = 0x%08x", lastAddr, reg));
			reg = BREG_Read32(hReg,g_humaxGpioTable[i].addr);
		}

		reg &= ~(g_humaxGpioTable[i].mask);

/*20090309 :: POOH :: drv_gpio������ ���� humax_pinmux������ ����ϱ� ���� NC TP���� 0�� �ƴ� -1�� �����Ͽ� ���.
 *			GPIO�� default value�� ���������� �׳� setting�ϵ��� ��.
 */
		if((g_humaxGpioFunctionTable[i]) == HUMAX_GPIO_FUNCTION_NCTP)
		{
/*			reg |= ( 0 << (g_humaxGpioTable[i].shift) );
 */
		}
		else
		{
			reg |= ( (g_humaxGpioFunctionTable[i]) << (g_humaxGpioTable[i].shift) );
		}

		lastAddr = g_humaxGpioTable[i].addr;

		if( i == HUMAX_NUM_GPIO_PINS-1)
		{
			BREG_Write32 (hReg, lastAddr, reg);
			BDBG_ERR(("BREG_Write32 0x%08x = 0x%08x", lastAddr, reg));
		}
	}

#if 0
	/* Configure the AVD UARTS to debug mode.  AVD0_OL -> UART1, AVD1_IL -> UART2. */
	if(enableMII==NULL)
	{
		/* Configure the AVD UARTS to debug mode.  AVD0_OL -> UART1, AVD1_IL -> UART2. */
#if 0
		reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);

		reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel) |
				BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel));
#if defined(USE_NEXUS_UART)
		reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, NO_CPU);
		reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel, NO_CPU);
#else
		reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, SVD0_OL);
		reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel, SVD0_IL);
#endif
		BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

		reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
		reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
		reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable,
							  BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SUN);

		BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);
#endif

	    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);

		reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel));
#if defined(USE_NEXUS_UART)
		reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, NO_CPU);
#else
		reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, SVD0_OL);
#endif

		BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

		reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
		reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
		reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable,16 /*BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS*/);
		BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);
	}
#endif
	return BERR_SUCCESS;
}

void NEXUS_Platform_ChangePinmux(unsigned char ucPioNum, unsigned char ucFuncType)
{
	BREG_Handle hReg = g_pCoreHandles->reg;
	uint32_t reg=0;


	if( ucPioNum < NEXUS_NUM_GPIO_PINS )
	{
		/* Clear field */
		reg = BREG_Read32(hReg, g_humaxGpioTable[ucPioNum].addr);
		reg &= ~(g_humaxGpioTable[ucPioNum].mask);

		/* Set field */
		if((g_humaxGpioFunctionTable[ucPioNum]) != HUMAX_GPIO_FUNCTION_NCTP)
		{
			reg |= ( ucFuncType << (g_humaxGpioTable[ucPioNum].shift) );
		}

		BREG_Write32 (hReg, g_humaxGpioTable[ucPioNum].addr, reg);
	}

}

#else
#if NEXUS_PLATFORM_DOCSIS_IB_SUPPORT || NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
#define NEXUS_HAS_MPOD   1
#else
#define NEXUS_HAS_MPOD   0
#endif
/***************************************************************************
Summary:
    Configure pin muxes for the 97425 reference platform
Description:
    The core module must be initialized for this to be called
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;
    uint32_t reg;

    /* AON_GPIO 00..17 set by OS        */
    /* -------------------------------*/


    /* GPIO 000..001 set by OS        */
    /* -------------------------------*/
    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5
     * GPIO_000    : ENET_ACTIVITY(1)
     * GPIO_001    : ENET_LINK(1)
    */


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5
     * GPIO_002    : UART_RXD_0(2)
     * GPIO_003    : UART_TXD_0(2)
    */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_002)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_003)
            );


    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_002, 1) | /* EMAC0_RX_EN_CTL */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_003, 1);  /* EMAC0_RX_CLK */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6
     * GPIO_004    : UART_CTS_0(2)
     * GPIO_005    : UART_RTS_0(2)
     * GPIO_006    : UART_RXD_1(2)
     * GPIO_007    : UART_TXD_1(2)
     * GPIO_008    : UART_CTS_1(2)
     * GPIO_009    : UART_RTS_1(2)
     * GPIO_010    : UART_RXD_2(2)
     * GPIO_011    : UART_TXD_2(2)
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_004)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_005)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_006)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_007)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_008)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_009)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_010)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_011)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_004, 1) | /* EMAC0_RXD_00 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_005, 1) | /* EMAC0_RXD_01 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_006, 1) | /* EMAC0_RXD_02 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_007, 1) | /* EMAC0_RXD_03 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_008, 1) | /* EMAC0_TXD_CLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_009, 1) | /* EMAC0_TXD_EN_CTL */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_010, 1) | /* EMAC0_TXD_00 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_011, 1);  /* EMAC0_TXD_01 */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7
     * GPIO_012    : UART_CTS_2(2)
     * GPIO_013    : UART_RTS_2(2)
     * GPIO_014    : PKT_ERROR5(1)
     * GPIO_015    : PKT_VALID5(1)
     * GPIO_016    : PKT_CLK0(1)
     * GPIO_017    : PKT_DATA0(1)
     * GPIO_018    : PKT_SYNC0(1)
     * GPIO_019    : PKT_ERROR0(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_012)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_013)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_014)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_015)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_016)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_017)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_018)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_019)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_012, 1) | /* EMAC0_TXD_02 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_013, 1) | /* EMAC0_TXD_03 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_014, 2) | /* EMAC0_MDIO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_015, 2) | /* EMAC0_MDC */
#if (defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF) || defined(NEXUS_USE_7425_SATIPSW))
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_016, 2) | /* MTSIF0_CLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_017, 2) | /* MTSIF0_DATA0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_018, 2) | /* MTSIF0_SYNC */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_019, 2);  /* MTSIF0_DATA1 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_016, 1) | /* PKT_CLK0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_017, 1) | /* PKT_DATA0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_018, 1) | /* PKT_SYNC0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_019, 0);  /* GPIO interrupt for 3112_tuner0*/
#endif


    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8
     * GPIO_020    : PKT_VALID0(1)
     * GPIO_021    : PKT_CLK1(1)
     * GPIO_022    : PKT_DATA1(1)
     * GPIO_023    : PKT_SYNC1(1)
     * GPIO_024    : PKT_ERROR1(1)
     * GPIO_025    : PKT_VALID1(1)
     * GPIO_026    : PKT_ERROR2(1)
     * GPIO_027    : PKT_VALID2(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_020)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_021)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_022)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_023)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_025)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_027)
            );

    reg |=
#if (defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF) || defined(NEXUS_USE_7425_SATIPSW))
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_020, 2) | /* MTSIF0_VALID */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_021, 2) | /* MTSIF0_DATA2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_022, 2) | /* MTSIF0_DATA3 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_023, 2) | /* MTSIF0_DATA4 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_020, 0) | /* GPIO interrupt for 3112_tuner1*/
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_021, 1) | /* PKT_CLK1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_022, 1) | /* PKT_DATA1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_023, 1) | /* PKT_SYNC1 */
#endif

#if (defined(NEXUS_PLATFORM_7425_CABLE) || defined(NEXUS_PLATFORM_7422_CABLE))
#if  NEXUS_USE_7425_VMS_SFF || NEXUS_USE_7422_VMS_SFF
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024, 2) | /* MTSIF0_DATA5 */
#elif !defined(NEXUS_USE_7425_SV_BOARD)
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024, 0) | /* GPIO interrupt for 3112_tuner2*/
#endif
#else
#if defined(NEXUS_USE_7425_SATIPSW)
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024, 2) | /* PKT_ERROR1 */
#else
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024, 1) | /* PKT_ERROR1 */
#endif
#endif

#if (defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF) || defined(NEXUS_USE_7425_SATIPSW))
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_025, 2) | /* MTSIF_ATS_RST */
#else
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_025, 1) | /* PKT_VALID1 */
#endif
#if (defined(NEXUS_PLATFORM_7425_CABLE) || defined(NEXUS_PLATFORM_7422_CABLE))
#if  NEXUS_USE_7425_VMS_SFF || NEXUS_USE_7422_VMS_SFF
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026, 2) | /* MTSIF0_DATA6 */
#elif NEXUS_USE_7425_SV_BOARD
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026, 0) | /* GPIO interrupt for 3112_tuner2*/
#endif
#else
#if defined(NEXUS_USE_7425_SATIPSW)
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026, 2) | /* MTSIF0_DATA6 */
#else
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026, 1) |
#endif
#endif
#if  NEXUS_USE_7425_VMS_SFF || NEXUS_USE_7422_VMS_SFF || NEXUS_USE_7425_SATIPSW
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_027, 2);  /* MTSIF0_DATA7 */
#else
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_027, 0);  /* GPIO interrupt for 3112_tuner3*/
#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9
     * GPIO_028    : PKT_DATA3(1)
     * GPIO_029    : PKT_SYNC3(1)
     * GPIO_030    : POD2CHIP_MCLKI(1)
     * GPIO_031    : CHIP2POD_MCLKO(1)
     * GPIO_032    : CHIP2POD_SCLK(1)
     * GPIO_033    : POD2CHIP_SDI(1)
     * GPIO_034    : CHIP2POD_SDO(1)
     * GPIO_035    : CHIP2POD_SCTL(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_028)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_029)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_030)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_031)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_032)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_033)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_034)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_035)
            );
#if (defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF))
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_028, 0) | /* BCM3128 GPIO interrupt*/
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_028, 1) | /* PKT_DATA3 */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_029, 1) | /* PKT_SYNC3 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_030, 0) | /* POD2CHIP_MCLKI */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_031, 0) | /* GPIO_031 */

#ifdef NEXUS_USE_7425_SATIPSW
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_032, 2) | /* SPI_M_SCK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_033, 2) | /* SPI_M_MOSI */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_034, 1) | /* CHIP2POD_SDO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_035, 2);  /* SPI_M_SS0B */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_032, 1) | /* CHIP2POD_SCLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_033, 1) | /* POD2CHIP_SDI */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_034, 1) | /* CHIP2POD_SDO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_035, 1);  /* CHIP2POD_SCTL */
#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10
     * GPIO_036    : POD2CHIP_MISTRT(1)
     * GPIO_037    : POD2CHIP_MIVAL(1)
     * GPIO_038    : POD2CHIP_MDI0(1)
     * GPIO_039    : POD2CHIP_MDI1(1)
     * GPIO_040    : POD2CHIP_MDI2(1)
     * GPIO_041    : POD2CHIP_MDI3(1)
     * GPIO_042    : POD2CHIP_MDI4(1)
     * GPIO_043    : POD2CHIP_MDI5(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_036)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_037)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_038)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_039)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_040)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_041)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_042)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_043)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_036, 1) | /* POD2CHIP_MISTRT */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_037, 0) | /* GPIO_37 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_038, 1) | /* POD2CHIP_MDI0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_039, 1) | /* POD2CHIP_MDI1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_040, 1) | /* POD2CHIP_MDI2 */
        #if NEXUS_HAS_MPOD
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_041, 1) | /* POD2CHIP_MDI3 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_042, 1) | /* POD2CHIP_MDI4 */
        #else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_041, 5) | /* TP_IN_21  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_042, 5) | /* TP_OUT_22 */
        #endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_043, 1);  /* POD2CHIP_MDI5 */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11
     * GPIO_044    : POD2CHIP_MDI6(1)
     * GPIO_045    : POD2CHIP_MDI7(1)
     * GPIO_046    : POD2CHIP_MICLK(1)
     * GPIO_047    : CHIP2POD_MOVAL(1)
     * GPIO_048    : CHIP2POD_MOSTRT(1)
     * GPIO_049    : CHIP2POD_MDO0(1)
     * GPIO_050    : CHIP2POD_MDO1(1)
     * GPIO_051    : CHIP2POD_MDO2(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_044)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_045)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_046)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_047)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_048)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_049)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_050)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_051)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_044, 1) | /* POD2CHIP_MDI6 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_045, 1) | /* POD2CHIP_MDI7 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_046, 1) | /* POD2CHIP_MICLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_047, 0) | /* GPIO_47 */

#if NEXUS_PLATFORM_7425_DBS
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_048, 0) | /* CHIP2POD_MOSTRT */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_049, 0) | /* CHIP2POD_MDO0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_050, 0) | /* CHIP2POD_MDO1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_051, 0); /* CHIP2POD_MDO2 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_048, 1) | /* CHIP2POD_MOSTRT */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_049, 1) | /* CHIP2POD_MDO0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_050, 1) | /* CHIP2POD_MDO1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_051, 1);  /* CHIP2POD_MDO2 */
#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12
     * GPIO_052    : CHIP2POD_MDO3(1)
     * GPIO_053    : CHIP2POD_MDO4(1)
     * GPIO_054    : CHIP2POD_MDO5(1)
     * GPIO_055    : CHIP2POD_MDO6(1)
     * GPIO_056    : CHIP2POD_MDO7(1)
     * GPIO_057    : CHIP2POD_MOCLK(1)
     * GPIO_058    : PKT_CLK2(1)
     * GPIO_059    : PKT_DATA2(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_052)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_053)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_054)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_055)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_056)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_057)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_058)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_059)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_052, 1) | /* CHIP2POD_MDO3 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_053, 1) | /* CHIP2POD_MDO4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_054, 1) | /* CHIP2POD_MDO5 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_055, 1) | /* CHIP2POD_MDO6 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_056, 1) | /* CHIP2POD_MDO7 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_057, 1) | /* CHIP2POD_MOCLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_058, 1) | /* PKT_CLK2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_059, 1);  /* PKT_DATA2 */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13
     * GPIO_060    : PKT_SYNC2(1)
     * GPIO_061    : PKT_CLK3(1)
     * GPIO_062    : ALT_TP_IN_25(6)
     * GPIO_063    : TP_OUT_31(6)
     * GPIO_064    : PKT_CLK4(1)
     * GPIO_065    : PKT_DATA4(1)
     * GPIO_066    : PKT_SYNC4(1)
     * GPIO_067    : ALT_TP_IN_29(7)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_060)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_061)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_062)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_063)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_064)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_065)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_066)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_067)
            );

    reg |=
#if (defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF))
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_060, 2) | /* MTSIF1_DATA0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_061, 2) | /* MTSIF1_SYNC */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_062, 2) | /* MTSIF1_DATA1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_063, 2) | /* MTSIF1_VALID */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_064, 2) | /* MTSIF1_DATA2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_065, 2) | /* MTSIF1_DATA3 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_066, 2) | /* MTSIF1_DATA4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_067, 2); /* MTSIF1_ATS_INC */

#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_060, 1) | /* PKT_SYNC2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_061, 1) | /* PKT_CLK3 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_062, 0) | /* GPIO interrupt for tuner4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_063, 0) | /* GPIO interrupt for tuner5*/
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_064, 1) | /* PKT_CLK4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_065, 1) | /* PKT_DATA4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_066, 1) | /* PKT_SYNC4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_067, 7);  /* UART1->UART_DEBUG6_RX->ALT_TP_IN_29 */

#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14
     * GPIO_068    : TP_OUT_27(7)
     * GPIO_069    : PKT_CLK5(1)
     * GPIO_070    : PKT_DATA5(1)
     * GPIO_071    : PKT_SYNC5(1)
     * GPIO_072    : PKT_CLK6(1)
     * GPIO_073    : PKT_DATA6(1)
     * GPIO_074    : PKT_SYNC6(1)
     * GPIO_075    : PKT_ERROR6(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_068)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_069)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_070)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_071)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_072)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_073)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_074)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_075)
            );

    reg |=
#if (defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF) || defined(NEXUS_USE_7425_SATIPSW))
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_068, 2) |  /* MTSIF1_ATS_RST */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_069, 2) |  /* MTSIF1_DATA6 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_070, 2) |  /* MTSIF1_DATA7 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_071, 2) |  /* MTSIF1_DATA5 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_068, 7) | /* UART1->UART_DEBUG6_TX->TP_OUT_27 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_069, 1) | /* PKT_CLK5 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_070, 1) | /* PKT_DATA5 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_071, 1) | /* PKT_SYNC5 */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_072, 1) | /* PKT_CLK6 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_073, 1) | /* PKT_DATA6 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_074, 1) | /* PKT_SYNC6 */
#ifdef NEXUS_USE_7425_SATIPSW
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_075, 0);  /* GPIO_075 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_075, 1);  /* PKT_ERROR6 */
#endif
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15
     * GPIO_076    : PKT_VALID6(1)
     * GPIO_077    : PKT_CLK7(1)
     * GPIO_078    : PKT_DATA7(1)
     * GPIO_079    : PKT_SYNC7(1)
     * GPIO_080    : TP_IN_04(6)
     * GPIO_081    : ALT_TP_OUT_11(6)
     * GPIO_082    : CODEC_SDI(1)
     * GPIO_083    : CODEC_SCLK(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_076)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_077)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_078)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_079)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_082)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_083)
            );

#ifdef NEXUS_USE_7425_SATIPSW
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_076, 0) | /* GPIO_076 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_077, 3) | /* EXT_IRQb_0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_078, 3) | /* EXT_IRQb_1 */
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_076, 1) | /* PKT_VALID6 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_077, 1) | /* PKT_CLK7 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_078, 1) | /* PKT_DATA7 */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_079, 1) | /* PKT_SYNC7 */
#if NEXUS_PLATFORM_4538_DBS
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_082, 3) | /* EXT_IRQb for 4538 daughter card */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_082, 0) | /* GPIO interrupt for 3128 daughter card. */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_083, 1);  /* CODEC_SCLK */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16
     * GPIO_084    : CODEC_FSYNCB(1)
     * GPIO_085    : CODEC_MCLK(1)
     * GPIO_086    : CODEC_SDO(1)
     * GPIO_087    : MOCA_ACTIVITY(1)
     * GPIO_088    : MOCA_LINK(1)
     * GPIO_089    : SC0_IO(1)
     * GPIO_090    : SC0_CLK(1)
     * GPIO_091    : SC0_RST(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_084)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_085)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_086)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_087)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_088)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_089)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_090)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_091)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_084, 1) | /* CODEC_FSYNCB */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_085, 1) | /* CODEC_MCLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_086, 1) | /* CODEC_SDO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_087, 1) | /* MOCA_ACTIVITY */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_088, 1) | /* MOCA_LINK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_089, 1) | /* SC0_IO */
#if NEXUS_HAS_MPOD
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_090, 0) | /* GPIO_90 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_091, 0);  /* GPIO_91 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_090, 1) | /* SC0_CLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_091, 1);  /* SC0_RST */
#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17
     * GPIO_092    : SC0_PRES(1)
     * GPIO_093    : SC0_VCC(1)
     * GPIO_094    : SC1_IO(1)
     * GPIO_095    : SC1_CLK(1)
     * GPIO_096    : SC1_RST(1)
     * GPIO_097    : SC1_PRES(1)
     * GPIO_098    : SC1_VCC(1)
     * GPIO_103    : RMX_DATA0(3)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_092)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_093)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_094)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_095)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_096)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_097)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_098)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_103)
            );

    reg |=
#if NEXUS_HAS_MPOD
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_092, 0) | /* GPIO_092*/
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_093, 0) | /* GPIO_093*/
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_092, 1) | /* SC0_PRES */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_093, 1) | /* SC0_VCC */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_094, 1) | /* SC1_IO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_095, 1) | /* SC1_CLK */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_096, 1) | /* SC1_RST */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_097, 1) | /* SC1_PRES */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_098, 1) | /* SC1_VCC */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_103, 3);  /* RMX_DATA0 */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18
     * GPIO_104    : RMX_SYNC0(3)
     * GPIO_105    : RMX_CLK1(3)
     * GPIO_106    : RMX_DATA1(3)
     * GPIO_107    : RMX_SYNC1(3)
     * GPIO_108    : UHF_LNA_PWRDN(3)
     * GPIO_109    : PWM_2(3)
     * GPIO_110    : BT_CLK_26087(3)
     * SGPIO_00    : BSC_M2_SCL(1)
     */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_104)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_105)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_106)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_107)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_108)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_109)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_110)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_104, 3) | /* RMX_SYNC0 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_105, 3) | /* RMX_CLK1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_106, 3) | /* RMX_DATA1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_107, 3) | /* RMX_SYNC1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_108, 3) | /* UHF_LNA_PWRDN */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_109, 3) | /* PWM_2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_110, 3) | /* BT_CLK_26087 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00, 1);  /* BSC_M2_SCL */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, reg);


    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19
     * SGPIO_01    : BSC_M2_SDA(1)
     * SGPIO_02    : BSC_M3_SCL(1)
     * SGPIO_03    : BSC_M3_SDA(1)
     * A0/A1:
     * SGPIO_04    : BSC_M4_SCL(1)
     * SGPIO_05    : BSC_M4_SDA(1)
     */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19);
#if BCHP_VER < BCHP_VER_B0
    reg &=~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_04 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_05 )
            );

    reg |=(
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01, 1 ) |  /* BSC_M2_SDA */
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02, 1 ) |  /* BSC_M3_SCL */
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03, 1 ) |  /* BSC_M3_SDA */
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_04, 1 ) |  /* BSC_M4_SCL */
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_05, 1 )    /* BSC_M4_SDA */
           );
#else
    reg &=~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03 )
            );

    reg |=(
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01, 1 ) |  /* BSC_M2_SDA */
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02, 1 ) |  /* BSC_M3_SCL */
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03, 1 )    /* BSC_M3_SDA */
           );
#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, reg);


    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0
     * AON_GPIO_00    : LED_KD_0(1)
     * AON_GPIO_01    : LED_KD_1(1)
     * AON_GPIO_02    : LED_KD_2(1)
     * AON_GPIO_03    : LED_KD_3(1)
     * AON_GPIO_04    : LED_LS_0(1)
     For Ax:
     * AON_GPIO_05    : LED_LS_1(1)
     * AON_GPIO_06    : LED_LS_2(1)
     For Bx:
     * AON_GPIO_05    : UART_RXD_0(2)
     * AON_GPIO_06    : UART_TXD_0(2)

     */
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    reg &=~(
#if BCHP_VER >= BCHP_VER_B0
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_07 ) |
#endif
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_06 )
            );

    reg |=(
#if BCHP_VER >= BCHP_VER_B0
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_07, 1 ) |  /* LED_LS_3 */
#endif
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00, 1 ) |  /* LED_KD_0 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01, 1 ) |  /* LED_KD_1 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02, 1 ) |  /* LED_KD_2 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03, 1 ) |  /* LED_KD_3 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04, 1 ) |  /* LED_LS_0 */
#if BCHP_VER >= BCHP_VER_B0
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 2 ) |  /* LED_LS_1 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_06, 2 )    /* LED_LS_2 */
#else
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 1 ) |  /* UART_RXD_0(2) */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_06, 1 )    /* UART_TXD_0(2) */
#endif
           );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);

    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1
     * AON_GPIO_07    : LED_LS_3(1)
     * AON_GPIO_08    : LED_LS_4(1)
     * AON_GPIO_09    : LED_LD_0(1)
     * AON_GPIO_10    : LED_LD_1(1)
     * AON_GPIO_11    : LED_LD_2(1)
     * AON_GPIO_12    : LED_LD_3(1)
     * AON_GPIO_13    : LED_LD_4(1)
     * AON_GPIO_14    : LED_LD_5(1)
     */
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
    reg &=~(
#if BCHP_VER < BCHP_VER_B0
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07 ) |
#else
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_15 ) |
#endif
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_14 )
            );

    reg |=(
#if BCHP_VER < BCHP_VER_B0
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07, 1 ) |  /* LED_LS_3 */
#else
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_15, 1 ) |  /* LED_LD_6 */
#endif
#if defined(NEXUS_USE_7425_SV_BOARD)
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08, 0 ) |  /* RFM Channel Select */
#else
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08, 1 ) |  /* LED_LS_4 */
#endif
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09, 1 ) |  /* LED_LD_0 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10, 1 ) |  /* LED_LD_1 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11, 1 ) |  /* LED_LD_2 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12, 1 ) |  /* LED_LD_3 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13, 0 ) |  /* LED_LD_4 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_14, 0 )    /* LED_LD_5 */
           );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, reg);


    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2
     * AON_GPIO_15    : LED_LD_6(1)
     * AON_GPIO_16    : LED_LD_7(1)
     * AON_GPIO_17    : LED_OUT(1)
     * GPIO_099       : PWM_0(1)
     * GPIO_100       : PWM_1(1)
     * GPIO_101       : IR_INT(1)
     * GPIO_102       : RMX_CLK0(3)
     * A0/A1:
     * AON_SGPIO_00   : BSC_M0_SCL(1)
     * B0:
     * AON_SGPIO_00   : BSC_M0_SCL(1)
     * AON_SGPIO_01   : BSC_M0_SDA(1)
     */
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);
    reg &=~(
#if BCHP_VER < BCHP_VER_B0
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_15 ) |
#else
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_01 ) |
#endif
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_099 )    |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_100 )    |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_101 )    |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_102 )    |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )
            );

    reg |=(
#if BCHP_VER < BCHP_VER_B0
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_15, 1 ) |  /* LED_LD_6 */
#else
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_01, 1 ) |  /* BSC_M0_SDA */
#endif
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16, 1 ) |  /* LED_LD_7 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17, 1 ) |  /* LED_OUT */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_099,    1 ) |  /* PWM_0 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_100,    1 ) |  /* PWM_1 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_101,    1 ) |  /* IR_INT */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_102,    3 ) |  /* RMX_CLK0 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00, 1 )   /* BSC_M0_SCL */
           );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);


    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3
     * AON_SGPIO_02   : BSC_S1_SCL(2)
     * A0/A1:
     * AON_SGPIO_01   : BSC_M0_SDA(1)
     * B0:
     * AON_SGPIO_04   : BSC_M4_SCL(1)
     * AON_SGPIO_05   : BSC_M4_SDA(1)
     */
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3);
    reg &=~(
#if BCHP_VER < BCHP_VER_B0
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01 ) |
#else
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05 ) |

#endif
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03 )
            );

    reg |=(
#if BCHP_VER < BCHP_VER_B0
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01, 1 ) |  /* BSC_M0_SDA */
#else
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04, 1 ) |  /* BSC_M4_SCL */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05, 1 ) |  /* BSC_M4_SDA */

#endif
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02, 2 ) |  /* BSC_S1_SCL */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03, 2 )    /* BSC_S1_SDA */
           );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, reg);


    /* Configure the AVD UARTS to debug mode.  AVD0_OL -> UART1, AVD1_OL -> UART2. */
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel) | BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel));
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel, SVD0_OL);
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel, AUDIO_FP);
    BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

    return BERR_SUCCESS;
}

#endif
