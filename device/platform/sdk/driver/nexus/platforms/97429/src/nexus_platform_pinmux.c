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
* $brcm_Date: 12/10/12 10:37a $
*
* Module Description:
*    This file contains pinmux initialization for the 97231 reference board.
* Revision History:
*
* $brcm_Log: /nexus/platforms/97429/src/nexus_platform_pinmux.c $
*
* 23   12/10/12 10:37a jrubio
* SW7429-410: fix compile issue for USFF board
*
* 22   12/6/12 5:56p mphillip
* SW7429-404: Add support for the 7429 + 4538 SATIP board
*
* 21   10/31/12 2:11p jessem
* SW7429-339: Added support for 7241_80211ACUSFF.
*
* 20   9/11/12 5:26p jowo
* SW7429-272: remove comment for gpio_078 in
*  NEXUS_Platform_P_InitPinmux()
*
* 19   9/11/12 2:41p ayxiang
* SW7429-272: fixed the problem MPOD working with Trinity again. Pinmux
*  nexus  init should be GPIO on GPIO_078.
*
* 18   7/6/12 5:07p katrep
* SW7429-127:DVB CI updates from Ashwin
*
* 17   6/26/12 3:56p vishk
* SW7429-183: Add support for 7241DCSFBTSFF board
*
* 16   6/25/12 4:31p katrep
* SW7429-1:minor updates
*
* 15   6/7/12 10:24a vishk
* SW7429-182: Incorrect pinmuxing for 97241 C2SFF platform.
*
* 14   5/7/12 6:00p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
*
* 13   4/26/12 2:46p katrep
* SW7429-87:avd ol uart for 97429
*
* 12   3/19/12 11:04a ayxiang
* SW7429-99: fixed the problem MPOD working with Trinity. Pinmux nexus
*  init should be GPIO on GPIO_078.
*
* 11   12/16/11 3:10p katrep
* SW7429-1:enabled streamer iniput 1 for 7429
*
* 10   12/2/11 3:57p mward
* SW7429-1:  Fix 97241 compile error.
*
* 9   12/1/11 5:45p mward
* SW7429-1:  Fix compile error for 97428.
*
* 8   12/1/11 4:14p mward
* SW7429-1:  SPDIF output pinmux for 97428, 97429.
*
* 7   11/29/11 10:22a mward
* SW7429-1:  7428 UART1 gpio_065, gpio_066 in PIN_MUX_CTRL_8, not 11.
*
* 6   11/15/11 5:21p mward
* SW7429-1: AVD UART for 97428.
*
* 5   11/15/11 4:57p mward
* SW7429-1: AVD UART for 97429.  Diffs between 7241 and 7231.
*
* 4   11/14/11 8:09p katrep
* SW7429-1:gpio minmuxing for 97429,97428,97241
*
* 3   10/26/11 3:31p katrep
* SW7429-1:update for 97428
*
* 2   9/23/11 3:37p katrep
* SW7429-1:7429 support
*
* 1   8/9/11 8:01p katrep
* SW7429-1:initial checkin
*
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "priv/nexus_core.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"

BDBG_MODULE(nexus_platform_pinmux);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if PLATFORM == 97241
#else
#include "humax_pinmux.h"
#endif
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
extern const char g_humaxGpioFunctionTable[] ;
extern HUMAX_GpioTable g_humaxGpioTable[];

#include "bchp_usb_ohci.h"
#include "bchp_usb_ohci1.h"

static void  NEXUS_Platform_P_InitUSBPowerMode(void)
{
	uint32_t reg=0;
	BREG_Handle hReg = g_pCoreHandles->reg;

	/* OHCI port ¸¦ Per-Port Mode·Î ¼³Á¤ & OCP enable ... */
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

/*20090309 :: POOH :: drv_gpio¿¡¼­µµ °°?º humax_pinmuxÆÄ?Ï?» »ç¿ëÇÏ±â ?§ÇØ NC TP°ª?» 0?Ì ¾Æ´Ñ -1·Î º¯°æÇÏ¿© »ç¿ë.
 *			GPIOµµ default value¿Í °ø?ÏÇÏÁö¸¸ ±×³É settingÇÏµµ·Ï ÇÔ.
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

   /* to remove CEC line internal pull down (CEC line connection to AON GPIO 20 is W/A of 7241 CEC problem */
	reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1);
	reg &= ~(BCHP_MASK(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1 , aon_gpio_20_pad_ctrl ));
	reg |= BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1 , aon_gpio_20_pad_ctrl, 0 );   /* PULL_NONE CEC W/A */
	BREG_Write32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,reg);

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

#if PLATFORM == 97428
/***************************************************************************
Summary:
    Configure pin muxes for the 97231 reference platform
Description:
    The core module must be initialized for this to be called
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;
    uint32_t reg;

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_033)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_034)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_035)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_036)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_037)
            );


    reg |=  (BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_033, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_034, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_035, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_036, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_037, 2)
            );

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047)
            );
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046, 4) |  /* PKT1_VALID  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047, 3) ;   /* PKT1_CLK */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_048)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_049)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_050)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_051)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_052)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_053)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_054)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_055)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_048, 3) |  /* PKT1     */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_049, 3) |  /* PKT1     */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_050, 4) |  /* PM_GPIO_50    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_051, 3) |  /* PKT1   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_052, 3) |  /* PKT2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_053, 3) |  /* PKT2  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_054, 3) |  /* PKT2   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_055, 3);   /* PKT2   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_056)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_057)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_058)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_059)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_060)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_061)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_062)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_063)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_056, 3) |  /* PKT2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_057, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_058, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_059, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_060, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_061, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_062, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_063, 4) ;  /* SC0  */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_067)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_068)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_069)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_070)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_071)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064, 4) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_067, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_068, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_069, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_070, 4) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_071, 4) ;
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_072)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_073)
            );
    reg |= ( BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_072, 4) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_073, 2)  /* RMX0   */
            );

    /* aon GPIO */

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    reg &=~(BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00));

    reg |= BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00, 1 );  /* aud spdif o/p */

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);
    reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )
            );

    reg |=(
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00, 1 ) /* MSC_M)_SCL.FPGA*/
           );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3);
    reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05 )
            );

    reg |=(
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01, 1 ) |
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02, 2 ) |
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03, 2 ) |
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04, 1 ) |
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05, 1 )
           );
    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, reg);

#if !NEXUS_HAS_DVB_CI
    /* AVD UART */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_066)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065, 6) |  /* 6 = ALT_TP_OUT_02  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_066, 6) ;  /* 6 = ALT_TP_IN_02   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, reg);
#endif
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);

    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel));
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, SVD0_OL);
    BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable,BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

    return BERR_SUCCESS;
}
#elif PLATFORM == 97429
NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;
    uint32_t reg;
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_033)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_034)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_035)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_036)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_037)
            );


    reg |=  (BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_033, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_034, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_035, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_036, 2) |
             BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_037, 2)
            );

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047)
            );
#if NEXUS_PLATFORM_4538_DBS
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046, 0) |  /* GPIO_046 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047, 0) ;  /* GPIO_047 */
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046, 4) |  /* PKT1_VALID  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047, 3) ;   /* PKT1_CLK */
#endif
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_048)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_049)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_050)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_051)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_052)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_053)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_054)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_055)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_048, 3) |  /* PKT1     */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_049, 3) |  /* PKT1     */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_050, 4) |  /* PM_GPIO_50    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_051, 3) |  /* PKT1   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_052, 3) |  /* PKT2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_053, 3) |  /* PKT2  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_054, 3) |  /* PKT2   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_055, 3);   /* PKT2   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, reg);
      reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_056)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_057)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_058)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_059)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_060)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_061)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_062)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_063)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_056, 3) |  /* PKT2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_057, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_058, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_059, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_060, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_061, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_062, 4) |  /* SC0  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_063, 4) ;  /* SC0  */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_067)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_068)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_069)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_070)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_071)
            );


    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064, 4) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_067, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_068, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_069, 5) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_070, 4) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_071, 4) ;
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_074)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_075)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_076)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_077)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_078)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_079)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_074, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_075, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_076, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_078, 0) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_079, 5) ;  /* NDS-SC0   */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_080)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082)   |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083)   |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084)   |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085)   |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086)   |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_080, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087, 3) ;
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_092)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_093)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088, 3) |  /* mtsif */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089, 3) |  /* mtsif */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091, 3) ;  /* MTSIF   */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104)
            );

    reg |= (
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101, 7)|  /* MTSIF */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102, 7)|  /* MTSIF */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103, 4)|  /* mtsif */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104, 7)  /* MTSIF */
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, reg);

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
    reg &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_105);
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_105, 6);  /* MTSIF */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118)
            );
    reg |= (
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116, 7) | /* MTSIF */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117, 7) | /* MTSIF */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 7)   /* MTSIF */
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, reg);

    /* aon GPIO */

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    reg &=~(BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00));

    reg |= BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00, 1 ); /* aud spdif o/p */

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);
    reg &=~(
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )
        );

    reg |=(
       BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00, 1 ) /* MSC_M)_SCL.FPGA*/
       );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3);
    reg &=~(
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05 )
        );
    reg |=(
       BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01, 1 ) |
       BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02, 2 ) |
       BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03, 2 ) |
       BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04, 1 ) |
       BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05, 1 )
       );
    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, reg);

#if !NEXUS_HAS_DVB_CI
    /* AVD UART */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094, 5) |  /* 5 = ALT_TP_OUT_07  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095, 5) ;  /* 5 = ALT_TP_IN_07   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);
#endif
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);

    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_7_cpu_sel));
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_7_cpu_sel, SVD0_OL);
    BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable,BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

    return BERR_SUCCESS;
}

#elif PLATFORM == 97241
NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;
    uint32_t reg;

    /*
       BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0:gpio_00-gpio_07 programmed by CFE/OS
       BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1:gpio_08-gpio_15 programmed by CFE/OS
       BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2:gpio_16-gpio_25 programmed by CFE/OS
       BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3:gpio_26-gpio_33 programmed by CFE/OS
       BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4:gpio_34-gpio_41 programmed by CFE/OS
       BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5:gpio_42-gpio_45 programmed by CFE/OS
    */
#if USE_SPI_FRONTEND
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_015)
            );

    reg |=	BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_015, 2);

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, reg);
#endif

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2);

    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_016)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_023)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_016, 1) | /* EBI_WE1B */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_023, 1); /* EBI_CS3B      */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, reg);

#if USE_SPI_FRONTEND
	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);

	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_024)
			);

	reg |=	BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_024, 2);

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, reg);

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);

	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_038)  |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_039)
			);

	reg |=	BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_038, 2) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_039, 2);

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, reg);
#endif

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);

    reg &= ~(
#if USE_SPI_FRONTEND
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_040)  |
#endif
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047)
            );

#if USE_SPI_FRONTEND
	reg |=	BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_040, 2) |  /* EBI_ADDR14		 */
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046, 3) |  /* EBI_ADDR14		 */
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_046, 3) |  /* EBI_ADDR14       */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_047, 1)   /* POD2CHIP_MDI0    */
            ;   /* POD2CHIP         */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_048)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_049)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_050)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_051)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_052)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_053)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_054)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_055)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_048, 1) |  /* POD2CHIP_MDI1    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_049, 1) |
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_050, 1) |  /* POD2CHIP_MDI3    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_051, 1) |  /* POD2CHIP_MDI4    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_052, 1) |  /* POD2CHIP_MDI5    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_053, 1) |   /* POD2CHIP_MDI6   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_054, 1) |  /* POD2CHIP_MDI7    */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_055, 1) ;  /* POD2CHIP_MISTRT  */


    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, reg);



    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_056)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_057)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_058)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_059)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_060)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_061)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_062)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_063)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_056, 1) |  /* POD2CHIP_MIVAL   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_057, 1) |  /* CHIP2POD_MCLKO   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_058, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_059, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_060, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_061, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_062, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_063, 1);  /* POD2CHIP_MDI3   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_066)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_067)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_068)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_069)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_070)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_071)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_065, 1) |  /* POD2CHIP_MDI3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_066, 1) |  /* CHIP2POD_MOSTRT */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_067, 1) |  /* CHIP2POD_MOVAL  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_068, 3) |  /* EBI_ADDR13      */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_069, 3) |  /* EBI_ADDR12      */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_070, 2) |  /* EBI_ADDR2       */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_071, 3);   /* EBI_ADDR1       */
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_072)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_073)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_074)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_075)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_076)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_077)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_078)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_079)
            );

    reg |=
#if !NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_072, 2) |  /* EBI_ADDR0       */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_073, 1) |  /* MPOD SDI        */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_074, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_075, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_076, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_077, 1) |  /* RMX0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_078, 1) |  /* RMX0   */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_072, 3) | /* NDS-SC0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_079, 1) ;  /* NDS-SC0   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_080)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_080, 1) |  /* NDS-SC0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081, 1) |  /* NDS-SC0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082, 1) |  /* NDS-SC0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083, 1) |  /* NDS-SC0   */
#if NEXUS_PLATFORM_7241_DCSFBTSFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087, 3) ;  /* MTSIF   */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087, 1) ;  /* PKT1   */
#endif


    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, reg);


    /* gpio_0092-gpio_0093 uart0 set by CFE */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_090)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095)
            );
#if NEXUS_PLATFORM_7231_FBTSFF || NEXUS_PLATFORM_7231_EUSFF
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088, 1) |  /* PKT1   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089, 1) |  /* PKT1   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_090, 1) |  /* Legacy mode transport PKT1 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091, 1) |  /* Legacy mode transport PKT1 */
#elif NEXUS_PLATFORM_7241_DCSFBTSFF
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088, 3) |  /* MTSIF   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089, 3) |  /* MTSIF   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091, 3) |  /* MTSIF */
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088, 1) |  /* PKT1   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089, 1) |  /* PKT1   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_090, 2) |  /* PKT3   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091, 2) |  /* PKT3   */
#endif
#if NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094, 4) |  /* POD   */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094, 3) |  /* POD   */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095, 4) ;  /* POD   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);


    /* gpio_0106 set by OS/CFE */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_096)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_097)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_098)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_099)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104)
            );

    reg |= (
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_096, 4) |  /* POD   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_097, 4) |  /* POD2CHIP_MCLKI   */
#if NEXUS_PLATFORM_7241_C2SFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_098, 2) |  /* EXT_IRQ2 for 3462 & hmirx_nxp   */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_098, 1) |  /* GP98_SF_HOLDb/(GP98_BCM3128_IRQb/GP98_DCARD1_IRQb)   */
#endif
#if !NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_099, 1) |  /* GP99_SF_WPb/(GP99_POD_IRQb) */
#endif
#if NEXUS_PLATFORM_7241_DCSFBTSFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101, 7)|  /* MTSIF */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102, 7)|  /* MTSIF */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103, 4)|  /* MTSIF*/
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103, 1)|  /* PKT 2 */
#endif
#if NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104, 1)  /* PKT3_SYNC */
#elif NEXUS_PLATFORM_7241_DCSFBTSFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104, 7)  /* PKT3_SYNC */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104, 1)  /* PKT3_SYNC */
#endif
            );

    if(NEXUS_StrCmp(NEXUS_GetEnv("hddvi_width"), "30")==0)
    {
        BDBG_WRN(("enabling 30 bit hd-dvi"));
        reg |= ( BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102, 2)|  /* HD_DVI0_0 */
                 BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101, 2)  /* HD_DVI0_10 */
                );
    }
    else
    {
        reg |= ( BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102, 1)|  /* PKT 2 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101, 1)  /* PKT 2 */
            );
    }
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_105));
#if NEXUS_PLATFORM_7241_DCSFBTSFF
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_105, 6);  /* MTSIF */
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_105, 1);  /* PKT2_ERR */
#endif
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, reg);


    /* gpio_107,gpio_108,gpio_109,gpio_110,gpio_111,gpio_112,gpio_113,gpio_114
       set by CFE/OS for USB and ENET */


    /*  gpio_122 -  gpio_126 is TDB AON set by OS???? */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117)  |
#if NEXUS_HAS_DVB_CI
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_122)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_123)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_124)
#else
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118)
#endif
            );


    reg |= (
#if NEXUS_PLATFORM_7231_FBTSFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116, 1) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117, 1) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 1)   /* PKT_4 */
#elif NEXUS_PLATFORM_7241_DCSFBTSFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116, 7) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117, 7) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 7)   /* PKT_4 */
#elif NEXUS_PLATFORM_7231_EUSFF
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116, 5) | /* TBD *** PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117, 5) | /* TBD *** PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 5)   /* HD_DVI0_11 */
#elif NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116, 1) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117, 4) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 4) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_122, 0) | /* GPIO_122 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_123, 0) | /* GPIO_123 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_124, 0)  /* GPIO_124 */
#elif NEXUS_PLATFORM_7241_C2SFF
           BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 0)    /* PKT_4 */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_116, 1) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_117, 0) | /* PKT_4 */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_118, 4)   /* PKT_4 */
#endif
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, reg);


    /*  gpio_127 -  gpio_130 is TDB AON set by OS???? */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15);
    reg &= ~(
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_128)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_129)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_130)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_131)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_132)
            );


    reg |= (
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_128, 2) |   /* i2s input */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_129, 2) |   /* i2s input */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_130, 2) |   /* i2s input */
#if !NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_131, 1) |  /* TBD Schematics say GPIO_131/CARD_2/SDIO0_LED/VEC_HSYNC---> GP123_POD_VS1  */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_132, 3)   /* HD_DVIO */
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, reg);


    /* gpio_139 - gpio_147 MII set by OS */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_133)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_134)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_135)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_136)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_138)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_139)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_140)
            );

    reg |= (
#if NEXUS_HAS_DVB_CI
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_133, 2) |  /* HD_DVIO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_135, 2) |  /* HD_DVIO */
#else
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_133, 4) |  /* HD_DVIO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_135, 4) |  /* HD_DVIO */
#endif
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_134, 3) |   /* HD_DVIO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_136, 5) |  /* HD_DVIO */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_138, 4) |  /* HD_DVI */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_139, 0) |  /* tbd gpio */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_140, 3)   /* HD_DVI */
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, reg);



    /* set by OS GP149_MDC_ENET */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
    reg &= ~(
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_141)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_142)  |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_148) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_143) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_144) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_145) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_146) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_144) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_148)
             );

    reg |= (
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_141, 3) |  /* HD_DVI */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_142, 4) |   /* HD_DVI */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_143, 4) |   /*  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_144, 4) |   /*  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_145, 4) |   /*  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_146, 4) |   /*  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_147, 4) |   /*  */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_148, 3)     /*  */
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, reg);


    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18);
    reg &= ~(
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_01) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_02) |
             BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_03)
             );

    reg |= (
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00, 1) |   /* BSC_M3_SCL */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_01, 1) |   /* BSC_M3_SDA */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_02, 1) |   /* BSC_M4_SCL */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_03, 1)     /* BSC_M4_SDA */
            );
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, reg);

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05)
            );

      reg |=(
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00, 1 ) | /* aud spdif o/p */
#if NEXUS_PLATFORM_7241_DCSFBTSFF
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04, 4)  | /* 3128_IRQ*/
#elif NEXUS_PLATFORM_7241_80211ACUSFF
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04, 0)  | /* AON_GPIO_4 */
#else
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04, 7)  | /* HD_DVI0_17 */
#endif

#if NEXUS_PLATFORM_7241_80211ACUSFF
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 0)   /* AON_GPIO_5 */
#else
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 7)    /* HD_DVI0_16 */
#endif
            );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);


    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1
     *
    */
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
    reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_06 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13 )
            );

    reg |=(
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_06, 7 ) |  /* HD_DVI0_15 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07, 6 ) |  /* HD_DVI0_14 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08, 6 ) |  /* HD_DVI0_13 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09, 6 ) |  /* HD_DVI0_12 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12, 6 ) |  /* HD_DVI0_9 */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13, 6 )    /* HD_DVI0_8 */
           );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, reg);


    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2
     *
     */
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);
    reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_14 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_15 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_18 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_19 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_20 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )
            );

    reg |=(
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_14, 7 ) |  /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_15, 7 ) |  /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16, 7 ) |  /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17, 7 ) |  /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_18, 7 ) |  /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_19, 7 ) |   /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_20, 7 ) |   /* HD_DVI */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00, 1 )    /* BSC_M0_SCL */
           );
    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3);
    reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05 )
            );

    reg |=(
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01, 1 ) | /* BSC_M0_SDA */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04, 1 ) | /* (HDMI_Tx) BSC_M2_SCL */
           BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05, 1 )   /* (HDMI_Tx) BSC_M2_SDA */
           );
    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, reg);



#if 1
#if !NEXUS_HAS_DVB_CI
    /* AVD UARTS */
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
    reg &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094)  |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095)
            );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094, 4) |  /* 4 = ALT_TP_OUT_0   */
            BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095, 5) ;  /* 5 = ALT_TP_IN_0   */

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);
#endif
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);

    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel));
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, SVD0_OL);
    /* reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel, SVD0_IL);  */
    BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

   reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
   reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
   reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable,16);
   BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

#endif

    return BERR_SUCCESS;
}

#else
#error "Please define the PLATFORM variable"
#endif
#endif


