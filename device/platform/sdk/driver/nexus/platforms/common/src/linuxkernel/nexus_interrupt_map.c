/***************************************************************************
*     (c)2011-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_interrupt_map.c $
* $brcm_Revision: 29 $
* $brcm_Date: 9/17/12 6:07p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_interrupt_map.c $
* 
* 29   9/17/12 6:07p hauxwell
* SW7435-352 : push the correct version of the file
* 
* 28   9/17/12 1:14p hauxwell
* SW7435-352 : Nexus interrupt map needs changing to remove programming
*  of V3D L2
* 
* 27   3/29/12 4:17p erickson
* SW7401-4517: remove old code
* 
* 26   1/30/12 4:20p jrubio
* SW7420-1318: disable HIF for 7325
* 
* 25   1/13/12 3:14p vishk
* SW7344-248: 3123 Fails to initialize in Kernel Mode. Works in Usermode
* 
* 24   1/13/12 2:43p jrubio
* SW7420-1318: fix compile error ,
* 
* 23   1/9/12 11:45a randyjew
* SW7420-1318: Compile error for 7468. missing,
* 
* 22   1/9/12 11:34a mward
* SW7420-1318: Compile error for 7400. missing ,.
* 
* 21   1/6/12 12:30p mward
* SW7420-1318: Compile error. missing };.
* 
* 20   1/6/12 9:36a erickson
* SW7420-1318: NEXUS_Platform_P_IrqMap still required to manage L1's with
*  no L2's (for instance, external irqs)
* 
* 19   1/5/12 10:46a jessem
* SW7425-2082, SW7425-2090: Removed 7422/7425 IRQ table and
*  upg_cpu_handler().
*
* 18   1/4/12 6:19p mward
* SW7420-1318: remove NEXUS_Platform_P_IrqMap for 7435.
*
* 17   1/4/12 6:09p katrep
* SW7231-317:removed 7231 and 7429 from the interrupt table
*
* 16   1/4/12 3:43p erickson
* SW7420-1318: revert to previous type and use terminator for test
*
* 15   1/4/12 3:29p erickson
* SW7420-1318: remove NEXUS_Platform_P_IrqMap for some chips that have no
*  L2 special handler and no L1 resequencing. nexus will use
*  BINT_P_IntMap instead.
*
* 14   12/22/11 5:57p katrep
* SW7429-1:fixed interrupt for 7429
*
* 13   12/22/11 11:15a mward
* SW7435-7:  Add 7435.
*
* 12   12/9/11 11:59a jessem
* SW7425-1693: Added BVNM1 to interrupt table. Enabled V3D interrupt.
*
* 11   12/7/11 6:23p jtna
* SW7425-1902: add V3D interrupt for 7425 B0 and bump up others
*
* 10   12/2/11 3:55p gmohile
* SW7231-506 : Do not enable SYS_PM interrupt
*
* 9   11/17/11 4:22p bselva
* SW7358-193: Disable SYS_PM
*
* 8   11/17/11 5:20p xhuang
* SW7552-9: disable SYS_PM
*
* 7   11/15/11 3:19p erickson
* SW7425-1747: remove old code
*
* 6   11/2/11 11:17a katrep
* SW7429-1:kernelmode support
*
* 5   8/26/11 3:14p gmohile
* SW7425-533 : SYS_PM interrupt is handled by kernel
*
* 4   7/25/11 1:21p jhaberf
* SWDTV-8070: check in on behalf of mstefan. Resolve kernel panic for DTV
*  platforms
*
* 3   7/18/11 2:32p jhaberf
* SWDTV-7208: check in on behalf of mstefan. Merge of DTV kernel mode
*  changes with the main
*
* 2   5/16/11 4:55p erickson
* SW7335-549: remove code for shared UPG_CPU interrupt. OS has dedicated
*  L1's for UART.
*
* 1   5/3/11 5:10p erickson
* SW7420-1819: repartition so NFE is a standard feature
*
***************************************************************************/
#include "nexus_interrupt_map.h"

/*
Hydra Linux interrupt numbering scheme:

The linux kernel has a virtual numbering scheme. We'll refer to it as "linuxirq".
The Hydra has two level one interrupt registers: W0 and W1. They are both 32 bits, but
not all bits are using.
The following code is a mapping between the virtual linuxirq and the physical W0 and W1
registers. The NEXUS_Platform_P_IrqMap structure uses the physical bit number as an index, treating
W0 and W1 as a 64 bit register.
Therefore NEXUS_Platform_P_IrqMap[physical_bit].linux_irq will obtain the virtual linuxirq.

WARNING: The virtual numbering scheme skips over the unused bits. Unfortunately,
these bits have and will become used in future versions of the chip, therefore the
following mapping is likely to break in the future. It's easy to fix it if you understand
the mapping.

You can see that W0 bits 29, 30, and 31 cannot be used with the current scheme. They
are currently unused. They may be used in the future.

linuxirq 0 is unused.
linuxirq 1 corresponds to W0 bit 0.
linuxirq 2 corresponds to W0 bit 1.
...
For 7038Bx:
  linuxirq 29 corresponds to W0 bit 28.
  linuxirq 30 corresponds to W1 bit 0.
  linuxirq 31 corresponds to W1 bit 1.
For 7038Cx:
  linuxirq 29 corresponds to W0 bit 28.
  linuxirq 30 corresponds to W0 bit 29.
  linuxirq 31 corresponds to W1 bit 0.
  linuxirq 32 corresponds to W1 bit 1.
For 3560Ax:
  linuxirq 29 corresponds to W0 bit 28.
  linuxirq 30 corresponds to W0 bit 29.
  linuxirq 31 corresponds to W0 bit 30.
  linuxirq 32 corresponds to W0 bit 31.
  linuxirq 33 corresponds to W1 bit 0.
  linuxirq 34 corresponds to W1 bit 1.
*/
#if (BCHP_CHIP==7038)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {1,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BACH"},
    {7,  false, true, "IFD"},
    {8,  false, true, "GFX", true},
    {9,  false, true, "HDMI"},
    {10, false, true, "VDEC"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "BVNF2"},
    {0,  false, true, "ENET"},
    {17, false, true, "RFM"},
    {18,  false, true, "UPG_TMR"},
    {19, false, true, "UPG"},
    {20, false, true, "UPG_BSC"},
    {21, false, true, "UPG_SPI"},
    {0,  false, true, "UPG_UART0"},
    {23, false, true, "UPG_SC"},
    {0,  false, true, "SUN"},
    {25, false, true, "MTP1"},
    {26, false, true, "MTP2"},
    {0,  false, true, "USB"},

#if BCHP_VER >= BCHP_VER_C0
    {0,  false, true, ""},/* reserved 28 */
    {29, false, true, "BVNF3"},
    {0,  false, true, "SATA_PCIB"},
    {0,  false, true, ""},/* reserved 31 */
    {0,  false, true, ""},/* reserved 32 */

    /* W1 interrupt register, starting with bit 0. The fact that bit 0 is
    linuxirq 31 is because the kernel guys compacted the numbering.
    This changed from Bx-Cx and is likely to break again when new
    bits are added to the W0 in the future. See above for details. */
    {31, false, true, "PCI_0"}, /* bit 0 of W1 */
    {32, false, true, "PCI_1"}, /* bit 1 of W1 */
    {0,  false, true, "PCI_2"},
    {0,  false, true, ""}, /* reserved  */
    {35, false, true, "EXT_IRQ_0"},
    {36, false, true, "EXT_IRQ_1"},
    {37, false, true, "EXT_IRQ_2"},
    {38, false, true, "EXT_IRQ_3"},
    {39, false, true, "EXT_IRQ_4"},
    {0,  false, true, "PCI_SATA"},
    {41,  false, true, "EXT_IRQ_5"},
    {42,  false, true, "EXT_IRQ_6"},
    {0,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {0,  false, true, "EXT_IRQ_10"},
    {0,  false, true, "EXT_IRQ_11"},
    {0,  false, true, "EXT_IRQ_12"},
    {0,  false, true, "EXT_IRQ_13"},
    {0,  false, true, "EXT_IRQ_14"}
#else
    /* linux kernel skips these numbers */
    {0,  false, true, ""},/* reserved 28 */
    {0,  false, true, ""},/* reserved 29 */
    {0,  false, true, ""},/* reserved 30 */
    {0,  false, true, ""},/* reserved 31 */
    {0,  false, true, ""},/* reserved 32 */

    /* W1 interrupt register, starting with bit 0. The fact that bit 0 is
    linuxirq 30 is because the kernel guys compacted the numbering.
    This changed from A0->B0 and is likely to break again when new
    bits are added to the W0 in the future. See above for details. */
    {0,  false, true, "PCI_0"}, /* bit 0 of W1 */
    {31, false, true, "PCI_1"}, /* bit 1 of W1 */
    {0,  false, true, "PCI_2"},
    {0,  false, true, ""}, /* reserved */
    {0,  false, true, "EXT_IRQ_0"},
    {0,  false, true, "EXT_IRQ_1"},
    {36, false, true, "EXT_IRQ_2"},
    {0,  false, true, "EXT_IRQ_3"},
    {38, false, true, "EXT_IRQ_4"},
    {0,  false, true, "PCI_SATA"}
#endif
    {0,   false, false, NULL}
};

/*
          CPU0
  2:          0        BCM INTC  XPT_STATUS
  3:          0        BCM INTC  XPT_OVFL
  4:          0        BCM INTC  XPT_MSG
  5:          0        BCM INTC  XPT_ICAM
  6:         16        BCM INTC  BACH
  7:         62        BCM INTC  IFD
  8:        121        BCM INTC  GFX
  9:          0        BCM INTC  HDMI
 10:       6576        BCM INTC  VDEC
 11:      10122        BCM INTC  VEC
 12:          0        BCM INTC  BVNB
 13:      30181        BCM INTC  BVNF0
 14:          0        BCM INTC  BVNF1
 15:          6        BCM INTC  BVNF2
 16:      17809        BCM INTC  <NULL>
 17:         23        BCM INTC  RFM
 19:          0        BCM INTC  UPG
 20:      19180        BCM INTC  UPG_BSC
 21:          0        BCM INTC  UPG_SPI
 23:          0        BCM INTC  UPG_SC
 25:          0        BCM INTC  MTP1
 26:          0        BCM INTC  MTP2
 32:          0        BCM INTC  PCI_1
 37:         96        BCM INTC  EXT_IRQ_2
 60:          0        BCM INTC  ehci_hcd
 61:          0        BCM INTC  brcm-usb-ohci
 62:          0        BCM INTC  brcm-usb-ohci
 63:      24798        BCM UART  serial
 64:         18        BCM UART  serial
 65:     191035  BCM MIPS TIMER INT  timer

ERR:          0
*/
#elif (BCHP_CHIP==7400)
static void upg_cpu_handler(int irq);
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {1,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "BVNF2"},
    {0,  false, true, "ENET"},
    {17, false, true, "RFM"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG", false, upg_cpu_handler},
    {20, false, true, "UPG_BSC"},
    {21, false, true, "UPG_SPI"},
    {0,  false, true, "UPG_UART0"},
    {23, false, true, "UPG_SC"},
    {24,  false, true, "SUN"},
    {25, false, true, "UHF1"},
    {26, false, true, "UHF2"},
    {0,  false, true, "USB"},
    {28,  false, true, "MC"},
    {29, false, true, "BVNF3"},
    {0,  false, true, "SATA_PCIB"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAVE"},
    {33, false, true, "PCI_0"},
    {34, false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {36,  false, true, "PX3D"},
    {0, false, true, "EXT_IRQ_0"},
    {38, false, true, "EXT_IRQ_1"},
    {39, false, true, "EXT_IRQ_2"},
    {40, false, true, "EXT_IRQ_3"},
    {41, false, true, "EXT_IRQ_4"},
    {0,  false, true, "PCI_SATA"},
    {43,  false, true, "EXT_IRQ_5"},
    {44,  false, true, "EXT_IRQ_6"},
    {45,  false, true, "EXT_IRQ_7"},
    {46,  false, true, "EXT_IRQ_8"},
    {47,  false, true, "EXT_IRQ_9"},
    {48,  false, true, "EXT_IRQ_10"},
    {49,  false, true, "EXT_IRQ_11"},
    {50,  false, true, "EXT_IRQ_12"},
    {51,  false, true, "EXT_IRQ_13"},
    {52,  false, true, "EXT_IRQ_14"},
    {0,  false, true, "IPI0"},
    {0,  false, true, "IPI1"}, /* reserved  */
    {55,  false, true, "AVD1"},
    #if BCHP_VER == BCHP_VER_A0
    {0,  false, true, "MEMC16B"}, /* reserved  */
    #else
    {56,  false, true, "BVNF6"},
    #endif
    {0,  false, true, "USB_EHCI_1"}, /* reserved  */
    {0,  false, true, "SM"}, /* reserved  */
    {59, false, true, "PCR"},
    {60, false, true, "XPT_FE"},
    {61, false, true, "XPT_MSG"},
    {0,  false, true, "USB_EHCI"},
    {0,  false, true, "USB_OHCI_0"},
    {0,  false, true, "USB_OHCI_1"},
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7325)
static void upg_cpu_handler(int irq);
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, true, "ENET"},
    {17, false, true, "RFM"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG", false, upg_cpu_handler},
    {20, false, true, "UPG_BSC"},
    {21, false, true, "UPG_SPI"},
    {0,  false, true, "UPG_UART0"},
    {23, false, true, "UPG_SC"},
    {24,  false, true, "SUN"},
    {25, false, true, "UHF1"},
    {0, false, true, "RSVD"},
    {0,  false, true, "USB"},
    {28,  false, true, "MC"},
    {29, false, true, "BVNF3"},
    {30,  false, true, "FTM"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAVE"},
    {0, false, true, "PCI_0"},
    {0, false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {36,  false, true, "QPSK_RCVR0"},
    {0, false, true, "EXT_IRQ_0"},
    {0, false, true, "EXT_IRQ_1"},
    {0, false, true, "EXT_IRQ_2"},
    {0, false, true, "EXT_IRQ_3"},
    {0, false, true, "EXT_IRQ_4"},
    {0,  false, true, "RSVD"},
    {0,  false, true, "EXT_IRQ_5"},
    {0,  false, true, "EXT_IRQ_6"},
    {0,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {0,  false, true, "EXT_IRQ_10"},
    {0,  false, true, "EXT_IRQ_11"},
    {0,  false, true, "EXT_IRQ_12"},
    {0,  false, true, "EXT_IRQ_13"},
    {0,  false, true, "EXT_IRQ_14"},
    {53,  false, true, "IPI0"},
    {54,  false, true, "IPI1"}, /* reserved  */
    {55,  false, true, "XPT_MSG_STAT"},
    {56,  false, true, "BVNF6"},
    {57,  false, true, "AFEC_CORE"}, /* reserved  */
    {0,  false, true, "SM"}, /* reserved  */
    {59, false, true, "USB_OHCI_1"},
    {60, false, true, "XPT_PCR"},
    {61, false, true, "XPT_FE"},
    {0,  false, true, "USB_EHCI_0"},
    {0,  false, true, "USB_OHCI_1"},
    {0,  false, true, "USB_OHCI_0"},
    {65,  false, true, "SDS_RCVR0"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {0,  false, true, "MEMC1"},
    {69,  false, true, "SDS_RCVR_1"},
    {70,  false, true, "QPSK_RCVR1"},
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7335)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, true, "ENET"},
    {17, false, true, "RFM"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG"},
    {20, false, true, "UPG_BSC"},
    {21, false, true, "UPG_SPI"},
    {0,  false, true, "UPG_UART0"},
    {23, false, true, "UPG_SC"},
    {24,  false, true, "SUN"},
    {25, false, true, "UHF1"},
    {26, false, true, "SDS0_RCVR1"},
    {0,  false, true, "USB"},
    {28,  false, true, "MC"},
    {29, false, true, "BVNF3"},
    {0,  false, true, "SATA_PCIB"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAVE"},
    {0, false, true, "PCI_0"},
    {0, false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {36,  false, true, "SDS1_RCVR0"},
    {37, false, true, "EXT_IRQ_0"},
    {0, false, true, "EXT_IRQ_1"},
    {0, false, true, "EXT_IRQ_2"},
    {0, false, true, "EXT_IRQ_3"},
    {0, false, true, "EXT_IRQ_4"},
    {0,  false, true, "PCI_SATA"},
    {0,  false, true, "EXT_IRQ_5"},
    {0,  false, true, "EXT_IRQ_6"},
    {0,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {0,  false, true, "EXT_IRQ_10"},
    {0,  false, true, "EXT_IRQ_11"},
    {0,  false, true, "EXT_IRQ_12"},
    {0,  false, true, "EXT_IRQ_13"},
    {0,  false, true, "EXT_IRQ_14"},
    {0,  false, true, "IPI0"},
    {0,  false, true, "IPI1"},
    {55,  false, true, "SDS0_RCVR0"},
    {56,  false, true, "BVNF6"},
    {0,  false, true, "USB_EHCI_1"}, /* reserved  */
    {0,  false, true, "SM"}, /* reserved  */
    {59, false, true, "PCR"},
    {60, false, true, "XPT_FE"},
    {61, false, true, "XPT_MSG"},
    {0,  false, true, "USB_EHCI"},
    {0,  false, true, "USB_OHCI_0"},
    {0,  false, true, "USB_OHCI_1"},
    {0,  false, true, "ENET_MAC1"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {68,  false, true, "SDS1_RCVR1"},
    {69,  false, true, "QPSK_RCVR0"},
    {70,  false, true, "QPSK_RCVR1"},
    {71,  false, true, "AFEC_CORE0"},
    {72,  false, true, "AFEC_CORE1"},
    {73,  false, true, "MEMC1"},
    {74,  false, true, "FTM"},
    {75, false, true, "NMI"},
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7346)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {1,  false, true, "AIO"}, /* 0*/
    {2,  false, true, "BSP"},
    {3,  false, true, "BVNB"},
    {4,  false, true, "BVNF0"},
    {5,  false, true, "BVNF1"},
    {6,  false, true, "BVNF5"},
    {7,  false, true, "BVNM"},
    {8,  false, true, "CLKGEN"},
    {9, false, true, "EXT_IRQ0"},
    {10, false, true, "EXT_IRQ1"},
    {11, false, true, "EXT_IRQ2"},
    {12, false, true, "EXT_IRQ3"},
    {13, false, true, "EXT_IRQ4"},
    {0,  false, true, "EXT_IRQ5"},
    {0, false, true, "EXT_IRQ6"},
    {0,  false, true, "EXT_IRQ7"},
    {0, false, true, "EXT_IRQ8"},
    {18,  false, true, "EXT_IRQ9"},
    {0, false, true, "EXT_IRQ10"},
    {0,  false, true, "EXT_IRQ11"},
    {0, false, true, "EXT_IRQ12"},
    {0,  false, true, "EXT_IRQ13"},
    {0, false, true, "EXT_IRQ14"}, /*25*/
    {24,  false, true, "FTM"},   /*24*/
    {0,  false, true, "GENET0_A"},
    {0,  false, true, "GENET0_B"},
    {0,  false, true, "GENET1_A"},
    {0,  false, true, "GENET1_B"},
    {29, false, true, "GFX"},
    {30, false, true, "HDMI_TX"},
    {0,  false, true, "HIF"},
    {0, false, true, "HIF_SPI"},  /* 32*/
    {33, false, true, "IPI0"},
    {34, false, true, "IPI1"},
    {35, false, true, "MEMC0"},
    {0,  false, true, "MOCA"},
    {37, false, true, "NMI"},
    {38, false, true, "RAAGA"},
    {39, false, true, "RAAGA_FW"},
    {0,  false, true, "SATA0"},
    {0,  false, true, "SATA1"},
    {42,  false, true, "SDS0_AFEC"},
    {43,  false, true, "SDS0_RCVR0"},
    {44,  false, true, "SDS0_RCVR1"},
    {45,  false, true, "SDS0_TFEC"},
    {46,  false, true, "SDS1_AFEC"},
    {47,  false, true, "SDS1_RCVR0"},
    {48,  false, true, "SDS1_RCVR1"},
    {49,  false, true, "SDS1_TFEC"},
    {50, false, true, "SM"},
    {51, false, true, "SVD0"},
    {52, false, true, "SYS"},
    {53, false, true, "SYS_AON"},
    {0, false, true, "SYS_PM"},
    {55, false, true, "UHF"},
    {56, false, true, "UPG_AUX"},
    {57, false, true, "UPG_AUX_AON"},
    {58, false, true, "UPG_BSC"},
    {59, false, true, "UPG_BSC_AON"},
    {60, false, true, "UPG_MAIN"},
    {61, false, true, "UPG_MAIN_AON"},
    {62, false, true, "UPG_SC"},
    {63, false, true, "UPG_SPI"},
    {64, false, true, "UPG_TMR"},
    {0,  false, true, "UPG_UART0"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {0,  false, true, "USB0"}, /*68*/
    {0,  false, true, "USB0_EHCI0"},
    {0,  false, true, "USB0_EHCI1"},
    {0,  false, true, "USB0_OHCI0"},
    {0,  false, true, "USB0_OHCI1"},
    {0,  false, true, "USB1"},
    {0,  false, true, "USB1_EHCI0"},
    {0,  false, true, "USB1_EHCI1"},
    {0,  false, true, "USB1_OHCI0"},
    {0,  false, true, "USB1_OHCI1"}, /*77*/
    {78, false, true, "VEC"},
    {79,  false, true, "XPT_FE"},
    {80,  false, true, "XPT_MSG"},
    {81,  false, true, "XPT_MSG_STAT"},
    {82,  false, true, "XPT_OVFL"},
    {83,  false, true, "XPT_PCR"},
    {84,  false, true, "XPT_RAV"},
    {85,  false, true, "XPT_STATUS"}, /*85*/
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7344)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {1,  false, true, "AIO"}, /* 0*/
    {2,  false, true, "BSP"},
    {3,  false, true, "BVNB"},
    {4,  false, true, "BVNF0"},
    {5,  false, true, "BVNF1"},
    {6,  false, true, "BVNF5"},
    {7,  false, true, "BVNM"},
    {8,  false, true, "CLKGEN"},
    {9, false, true, "EXT_IRQ0"},
    {10, false, true, "EXT_IRQ1"},
    {11, false, true, "EXT_IRQ2"},
    {12, false, true, "EXT_IRQ3"},
    {13, false, true, "EXT_IRQ4"},
    {0,  false, true, "EXT_IRQ5"},
    {0, false, true, "EXT_IRQ6"},
    {0,  false, true, "EXT_IRQ7"},
    {0, false, true, "EXT_IRQ8"},
    {0,  false, true, "EXT_IRQ9"},
    {19, false, true, "EXT_IRQ10"}, /* 18 */
    {0,  false, true, "EXT_IRQ11"},
    {0, false, true, "EXT_IRQ12"},
    {0,  false, true, "EXT_IRQ13"},
    {0, false, true, "EXT_IRQ14"},
    {0, false, true, "EXT_IRQ15"},
    {0, false, true, "EXT_IRQ16"}, /*25*/
    {0, false, true, "EXT_IRQ17"}, /*25*/
    {0, false, true, "EXT_IRQ18"}, /*25*/
    {0, false, true, "EXT_IRQ19"}, /*25*/
    {0, false, true, "EXT_IRQ20"}, /*25*/
    {30,  false, true, "FTM"},   /*30*/
    {0,  false, true, "GENET0_A"},
    {0,  false, true, "GENET0_B"},
    {0,  false, true, "GENET1_A"},
    {0,  false, true, "GENET1_B"},
    {35, false, true, "GFX"}, /*35*/
    {36, false, true, "HDMI_TX"},
    {0,  false, true, "HIF"},
    {0, false, true, "HIF_SPI"},  /* 38*/
    {39, false, true, "IPI0"},
    {40, false, true, "IPI1"},
    {41, false, true, "MEMC0"},
    {0,  false, true, "MOCA"},
    {43, false, true, "NMI"},
    {44, false, true, "RAAGA"},
    {45, false, true, "RAAGA_FW"},
    {46,  false, true, "SDS0_AFEC"},
    {47,  false, true, "SDS0_RCVR0"},
    {48,  false, true, "SDS0_RCVR1"},
    {49,  false, true, "SDS0_TFEC"},
    {50, false, true, "SM"},
    {51, false, true, "SVD0"},
    {52, false, true, "SYS"},
    {53, false, true, "SYS_AON"},
    {0, false, true, "SYS_PM"},
    {55, false, true, "UHF"},
    {56, false, true, "UPG_AUX"},
    {57, false, true, "UPG_AUX_AON"},
    {58, false, true, "UPG_BSC"},
    {59, false, true, "UPG_BSC_AON"},
    {60, false, true, "UPG_MAIN"},
    {61, false, true, "UPG_MAIN_AON"},
    {62, false, true, "UPG_SC"},
    {63, false, true, "UPG_SPI"},
    {64, false, true, "UPG_TMR"},
    {0,  false, true, "UPG_UART0"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {0,  false, true, "USB0"}, /*68*/
    {0,  false, true, "USB0_EHCI0"},
    {0,  false, true, "USB0_EHCI1"},
    {0,  false, true, "USB0_OHCI0"},
    {0,  false, true, "USB0_OHCI1"},
    {0,  false, true, "USB1"},
    {0,  false, true, "USB1_EHCI0"},
    {0,  false, true, "USB1_EHCI1"},
    {0,  false, true, "USB1_OHCI0"},
    {0,  false, true, "USB1_OHCI1"}, /*77*/
    {78, false, true, "VEC"},
    {79,  false, true, "XPT_FE"},
    {80,  false, true, "XPT_MSG"},
    {81,  false, true, "XPT_MSG_STAT"},
    {82,  false, true, "XPT_OVFL"},
    {83,  false, true, "XPT_PCR"},
    {84,  false, true, "XPT_RAV"},
    {85,  false, true, "XPT_STATUS"}, /*85*/
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7420)
static void upg_cpu_handler(int irq);
static void mcif_cpu_handler(int irq);
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {1,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, true, "ENET"},
    {17, false, true, "RFM1"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG", false, upg_cpu_handler},
    {20, false, true, "UPG_BSC"},
    {21, false, true, "UPG_SPI"},
    {0,  false, true, "UPG_UART0"},
    {23, false, true, "UPG_SC"},
    {24,  false, true, "SUN"},
    {25, false, true, "UHF1"},
    {0, false, true, "FW1394"},
    {0,  false, true, "USB"},
    {28,  false, true, "MC", true, mcif_cpu_handler},
    {29, false, true, "BVNF3"},
    {30,  false, true, "SATA_PCIB"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAVE"},
    {33, false, true, "PCI_0"},
    {0, false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {36,  false, true, "RFM2"},
    {37, false, true, "EXT_IRQ_0"},
#if (defined(NEXUS_PLATFORM_7420_DBS) || (SMS93380_SUPPORT && (BCHP_VER >= BCHP_VER_C0)))
    {38, false, true, "EXT_IRQ_1"},
#else
    {0, false, true, "EXT_IRQ_1"},
#endif
    {39, false, true, "EXT_IRQ_2"},
    {40, false, true, "EXT_IRQ_3"},
    {41, false, true, "EXT_IRQ_4"},
    {42,  false, true, "PCI_SATA"},
    {43,  false, true, "EXT_IRQ_5"},
    {0,  false, true, "EXT_IRQ_6"},
    {45,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {48,  false, true, "EXT_IRQ_10"},
    {49,  false, true, "EXT_IRQ_11"},
    {50,  false, true, "EXT_IRQ_12"},
    {51,  false, true, "EXT_IRQ_13"},
    {52,  false, true, "EXT_IRQ_14"},
    {0,  false, true, "IPI0"},
    {0,  false, true, "IPI1"}, /* reserved  */
    {55,  false, true, "AVD1"},
    {0, false, true, "USB_OHCI_1"},
    {0,  false, true, "SM"}, /* reserved  */
    {58, false, true, "XPT_PCR"},
    {59, false, true, "XPT_FE"},
    {60, false, true, "XPT_MSG"},
    {0,  false, true, "USB_EHCI_0"},
    {0,  false, true, "USB_OHCI_0"},
    {0,  false, true, "USB_OHCI_1"},
    {64,  false, true, "UHF2"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {67,  false, true, "NMI"},
    {0,  false, true, "MOCA"},
    {0,  false, true, "MOCA_ENET"},
    {0,  false, true, "PM"},
    {71,  false, true, "MEMC1"},
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7342)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, true, "HIF"}, /* disable*/
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, true, "ENET"},
    {17, false, true, "PM"},
    {18, false, true, "FTM"},
    {19, false, true, "UPG_TMR"},
    {20, false, true, "UPG_CPU" },
    {21, false, true, "UPG_BSC"},
    {22,  false, true, "UPG_SPI"},
    {0, false, true, "UPG_UART0"},
    {24,  false, true, "UPG_SC"},
    {25, false, true, "SUN"},
    {26,  false, true, "SDS0_RCVR1"},
    {27,  false, true, "UHF1"},
    {0, false, true, "USB"},
    {29,  false, true, "MC"},
    {30, false, true, "BVNF5"},
    {0, false, true, "SATA"},
    {32, false, true, "AVD0"},
    {0, false, true, "PCI_0"},
    {0,  false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {36, false, true, "SDS1_RCVR0"},
    {37, false, true, "EXT_IRQ_0"},
    {0, false, true, "EXT_IRQ_1"},
    {0, false, true, "EXT_IRQ_2"},
    {0, false, true, "EXT_IRQ_3"},
    {0,  false, true, "EXT_IRQ_4"},
    {0,  false, true, "PCI_SATA"},
    {0,  false, true, "EXT_IRQ_5"},
    {0,  false, true, "EXT_IRQ_6"},
    {0,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {0,  false, true, "EXT_IRQ_10"},
    {0,  false, true, "EXT_IRQ_11"},
    {0,  false, true, "EXT_IRQ_12"},
    {0,  false, true, "EXT_IRQ_13"},
    {0,  false, true, "EXT_IRQ_14"},
    {0,  false, true, "IPI0"}, /* reserved  */
    {0,  false, true, "IPI1"},
    {55, false, true, "XPT_MSG"},
    {56,  false, true, "SDS0_RCVR0"},
    {57, false, true, "SDS1_RCVR1"},
    {0, false, true, "USB_EHCI_1"},
    {0, false, true, "USB_OHCI_1"},
    {60,  false, true, "XPT_PCR"},
    {61,  false, true, "XPT_FE"},
    {62,  false, true, "XPT_RAV"},
    {0,  false, true, "USB_EHCI_0"},
    {0,  false, true, "USB_OHCI_0"},
    {0,  false, true, "SM"},
    {66,  false, true, "AFEC_CORE0"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {69,  false, true, "NMI"},
    {70,  false, true, "QPSK_RCVR0"},
    {71,  false, true, "QPSK_RCVR1"},
    {0,  false, true, "MOCA_INTR"},
    {0,  false, true, "MOCA_GENET_A"},
    {74,  false, true, "AFEC_CORE1"},
    {0,  false, true, "GENET_0_B"},
    {0,  false, true, "MOCA_GENET_B"},
    {77, false, true, "BVNF_6"},
    {0,  false, true, "USB_EHCI_2"},
    {0,  false, true, "USB_OHCI_2"},
    {80,  false, true, "SDS0_TFEC"},
    {81,  false, true, "SDS1_TFEC"},
    {0,   false, false, NULL}
}; /* End 7342*/
#elif (BCHP_CHIP==7340)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, true, "ENET"},
    {0, false, true, "NULL"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG_CPU" },
    {20, false, true, "UPG_BSC"},
    {21,  false, true, "UPG_SPI"},
    {0, false, true, "UPG_UART0"},
    {23,  false, true, "UPG_SC"},
    {24, false, true, "SUN"},
    {25, false, true, "UHF1"},
    {26,  false, true, "NULL"},
    {0, false, true, "USB"},
    {28,  false, true, "MC"},
    {29, false, true, "BVNF5"},
    {30, false, true, "FTM"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAV"},
    {0, false, true, "PCI_0"},
    {0,  false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {36, false, true, "QPSK_RCVR0"},
    {37, false, true, "EXT_IRQ_0"},
    {0, false, true, "EXT_IRQ_1"},
    {0, false, true, "EXT_IRQ_2"},
    {0, false, true, "EXT_IRQ_3"},
    {0,  false, true, "EXT_IRQ_4"},
    {0,  false, true, "NULL"},
    {0,  false, true, "EXT_IRQ_5"},
    {0,  false, true, "EXT_IRQ_6"},
    {0,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {0,  false, true, "EXT_IRQ_10"},
    {0,  false, true, "EXT_IRQ_11"},
    {0,  false, true, "EXT_IRQ_12"},
    {0,  false, true, "EXT_IRQ_13"},
    {0,  false, true, "EXT_IRQ_14"},
    {53,  false, true, "IPI0"}, /* reserved  */
    {54,  false, true, "IPI1"},
    {55, false, true, "XPT_MSG"},
    {56,  false, true, "BVNF_6"},
    {57, false, true, "AFEC_CORE"},
    {0, false, true, "SM"},
    {0, false, true, "USB_OHCI_1"},
    {60,  false, true, "XPT_PCR"},
    {61,  false, true, "XPT_FE"},
    {0,  false, true, "USB_EHCI_0"},
    {0,  false, true, "USB_EHCI_1"},
    {0,  false, true, "USB_OHCI_0"},
    {65,  false, true, "SDS_RCVR0"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {0,  false, true, "MOCA_INTR"},
    {0,  false, true, "MOCA_GENET"},
    {70,  false, true, "QPSK_RCVR1"},
    {71,  false, true, "TFEC"},
    {72,  false, true, "SDS_RCVR1"},
    {73,  false, true, "NMI"},
    {74,  false, true, "SUNDRY_PM"},
    {0,   false, false, NULL}
}; /* END  7340 */
#elif (BCHP_CHIP==7468)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
/*W0*/
/*0*/   {0,  false, true, "HIF"},
/*1 */    {2,  false, true, "XPT_STATUS"},
/*2*/    {3,  false, true, "XPT_OVFL"},
/*3 */    {4,  false, true, "XPT_MSG"},
/*4 */    {5,  false, true, "XPT_ICAM"},
/*5 */    {6,  false, true, "BSP"},
/*6 */    {7,  false, true, "AIO"},
/*7 */    {8,  false, true, "M2MC"},
/*8 */    {9,  false, true, "HDMI"},
/*9 */    {10, false, true, "RPTD"},
/*10 */    {11, false, true, "VEC"},
/*11 */    {12, false, true, "BVNB"},
/*12 */    {13, false, true, "BVNF0"},
/*13 */    {14, false, true, "BVNF1"},
/*14*/    {15, false, true, "MEMC0"},
/*15 */    {16,  false, true, "GENET_0_A"},
/*16*/    {17, false, true, ""},
/*17*/    {18, false, true, "UPG_TMR"},
/*18*/    {19, false, true, "UPG_CPU"},
/*19*/    {20, false, true, "UPG_BSC" },
/*20*/    {21, false, true, ""},
/*21*/    {0,  false, true, "UPG_UART0"},
/*22 */    {23, false, true, "UPG_SC"},
/*23*/    {24, false, true, "SUN"},
/*24*/    {0,  false, true, ""},
/*25*/    {0,  false, true, ""},
/*26*/    {0,  false, true, "USB"},
/*27*/    {0,  false, true, "MC"},
/*28*/    {29, false, true, "BVNF5"},
/*29*/    {30,  false, true, "BVNF6"},
/*30*/    {31, false, true, "AVD0"},
/*31*/  {32, false, true, "XPT_RAV"},
/*W1*/
/*0 */    {0,  false, true, ""},
/*1 */    {0,  false, true, ""},
/*2 */    {0,  false, true, ""},
/*3 */  {0,  false, true, ""},
/*4 */    {0,  false, true, "EXT_IRQ_0"},
/*5 */    {0,  false, true, "EXT_IRQ_1"},
/*6 */    {0,  false, true, "EXT_IRQ_2"},
/*7 */    {0,  false, true, "EXT_IRQ_3"},
/*8 */    {0,  false, true, "EXT_IRQ_4"},
/*9 */    {0,  false, true, ""},
/*10 */    {0,  false, true, ""},
/*11 */    {0,  false, true, ""},
/*12 */    {0,  false, true, ""},
/*13 */    {0,  false, true, ""},
/*14 */    {0,  false, true, ""},
/*15 */    {0,  false, true, ""},
/*16 */    {0,  false, true, ""},
/*17 */    {0,  false, true, ""},
/*18*/    {0,  false, true, ""},
/*19*/    {0,  false, true, ""},
/*20 */    {53, false, true, "IPI0"}, /* reserved  */
/*21 */    {0, false, true, ""},
/*22*/    {0,  false, true, ""},
/*23 */    {0,  false, true, ""},
/*24*/    {0,  false, true, ""},
/*25*/  {58, false, true, "XPT_PCR"},
/*26*/  {59, false, true, "XPT_FE"},
/*27 */ {60, false, true, "XPT_MSG_STAT"},
/*28*/    {0,  false, true, "USB_EHCI_0"},
/*29*/    {0,  false, true, "USB_OHCI_0"},
/*30*/    {0,  false, true, ""},
/*31*/    {0,  false, true, ""},
/*W2*/
/*0 */    {0,  false, true, "UPG_UART1"},
/*1 */    {0,  false, true, ""},
/*2 */    {67, false, true, "NMI"},
/*3 */    {0,  false, true, ""},
/*4 */    {0,  false, true, ""},
/*5 */    {70, false, true, "SUN_PM"},
/*6 */    {0,  false, true, ""},
/*7 */    {0,  false, true, "HIF_SPI"},
/*8 */  {73,  false, true, "GENET_0_B"},
/*9 */  {0,  false, true, ""},
/*10 */ {0,  false, true, ""},
/*11 */ {0,  false, true, ""},
/*12 */ {0,  false, true, ""},
/*13*/  {0,  false, true, ""},
/*14*/  {0,  false, true, ""},
/*15*/  {0,  false, true, ""},
/*16*/  {0,  false, true, ""},
/*17*/  {0,  false, true, ""},
/*18*/  {0,  false, true, ""},
/*19*/  {0,  false, true, ""},
/*20*/  {0,  false, true, ""},
/*21*/  {0,  false, true, ""},
/*22*/  {0,  false, true, ""},
/*23*/  {0,  false, true, ""},
/*24*/  {0,  false, true, ""},
/*25*/  {0,  false, true, ""},
/*26*/  {0,  false, true, ""},
/*27*/  {0,  false, true, ""},
/*28*/  {0,  false, true, ""},
/*29*/  {0,  false, true, ""},
/*30*/  {0,  false, true, ""},
/*31*/  {0,  false, true, ""},
    {0,   false, false, NULL}
}; /* End 7468*/
#elif (BCHP_CHIP==7125)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, true, "HIF"},
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_OVFL"},
    {4,  false, true, "XPT_MSG"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "BSP"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {10, false, true, "RPTD"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, true, "HIF_SPI"},
    {17, false, true, ""},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG_CPU"},
    {20, false, true, "UPG_BSC" },
    {21, false, true, "UPG_SPI"},
    {0,  false, true, "UPG_UART0"},
    {23, false, true, "UPG_SC"},
    {24, false, true, "SUN"},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, "USB"},
    {0,  false, true, "MC"},
    {29, false, true, "BVNF5"},
    {0,  false, true, "SATA"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAV"},
    {0,  false, true, "PCI_0"},
    {0,  false, true, "PCI_1"},
    {0,  false, true, "PCI_2"},
    {0,  false, true, ""},
    {0,  false, true, "EXT_IRQ_0"},
    {0,  false, true, "EXT_IRQ_1"},
    {0,  false, true, "EXT_IRQ_2"},
    {0,  false, true, "EXT_IRQ_3"},
    {0,  false, true, "EXT_IRQ_4"},
    {0,  false, true, "PCI_SATA"},
    {0,  false, true, "EXT_IRQ_5"},
    {0,  false, true, "EXT_IRQ_6"},
    {0,  false, true, "EXT_IRQ_7"},
    {0,  false, true, "EXT_IRQ_8"},
    {0,  false, true, "EXT_IRQ_9"},
    {0,  false, true, "EXT_IRQ_10"},
    {0,  false, true, "EXT_IRQ_11"},
    {50,  false, true, "EXT_IRQ_12"},
    {51,  false, true, "EXT_IRQ_13"},
    {0,  false, true, "EXT_IRQ_14"},
    {53, false, true, "IPI0"}, /* reserved  */
    {54, false, true, "IPI1"},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {58, false, true, "XPT_PCR"},
    {59, false, true, "XPT_FE"},
    {60, false, true, "XPT_MSG_STAT"},
    {0,  false, true, "USB_EHCI_0"},
    {0,  false, true, "USB_OHCI_0"},
    {0,  false, true, ""},
    {0,  false, true, "BNM"},
    {0,  false, true, "UPG_UART1"},
    {0,  false, true, "UPG_UART2"},
    {67, false, true, "NMI"},
    {0,  false, true, "MOCA_INTR"},
    {0,  false, true, "MOCA_GENET_A"},
    {70, false, true, "SUN_PM"},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, ""},
    {0,  false, true, "MOCA_GENET_A"},
    {0,   false, false, NULL}
}; /* End 7125*/
#elif BCHP_CHIP == 7408
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, false, "HIF"}, /* disable*/
    {2,  false, true, "XPT_STATUS"},
    {3,  false, true, "XPT_MDMA"},
    {4,  false, true, "XPT_JTAG"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {7,  false, true, "AIO"},
    {8,  false, true, "GFX"},
    {9,  false, true, "HDMI"},
    {0,  false, false, "Reserved"},
    {11, false, true, "VEC"},
    {12, false, true, "BVNB"},
    {13, false, true, "BVNF0"},
    {14, false, true, "BVNF1"},
    {15, false, true, "MEMC0"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG_CPU" },
    {20, false, true, "UPG_BSC"},
    {0,  false, false, "Reserved"},
    {22, false, false, "UPG_UART0"},
    {0,  false, false, "Reserved"},
    {24, false, true, "SUN"},
    {25,  false, true, "UHF1"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "USB"},
    {0,  false, false, "MC"},
    {29, false, true, "BVNF5"},
    {0,  false, false, "Reserved"},
    {31, false, true, "AVD0"},
    {32, false, true, "XPT_RAV"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {37, false, true, "EXT_IRQ_0"},
    {38, false, false, "EXT_IRQ_1"},
    {39, false, true, "EXT_IRQ_2"},
    {40, false, true, "EXT_IRQ_3"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {53, false, true, "IPI0"}, /* reserved  */
    {54, false, true, "IPI1"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {58, false, true,  "XPT_PCR"},
    {59, false, true,  "XPT_FE"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "USB_EHCI_0"},
    {0,  false, false, "USB_OHCI_0"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "UPG_UART1"},
    {0,  false, false, "UPG_UART2"},
    {67, false, true,  "NMI_INTR"},
    {0,  false, false, "MOCA_INTR"},
    {0,  false, false, "MOCA_GENET_0"},
    {70, false, true, "SUNDRY_PM"},
    {0,  false, false, "Reserved"},
    {72, false, true, "BVNF6"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "HIF_SPI"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "MOCA_GENET_0"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,  false, false, "Reserved"},
    {0,   false, false, NULL}
}; /* End 7408*/
#elif BCHP_CHIP == 7550
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {0,  false, false, "HIF"}, /* disable*/
    {2,  false, false, "HIF_SPI"},
    {3,  false, true, "XPT_STATUS"},
    {4,  false, true, "XPT_RAV"},
    {5,  false, true, "XPT_ICAM"},
    {6,  false, true, "AIO_CPU"},
    {7,  false, true, "RPTD_CPU"},
    {8,  false, true, "DVP_HT"},
    {9,  false, true, "VEC"},
    {10,  false, true, "BVNB"},
    {11, false, true, "BVNF0"},
    {12, false, true, "BVNF1"},
    {13, false, true, "BVNF5"},
    {14, false, true, "MEMC0"},
    {15, false, false, "ENET"},
    {16,  false, true, "UPG_CPU"},
    {17,  false, true, "UPG_GPIO"},
    {18, false, true, "UPG_TMR"},
    {19, false, true, "UPG_BSC" },
    {20, false, true, "UPG_SPI"},
    {21,  false, false, "UPG_UART0"},
    {22, false, true, "UPG_SC"},
    {23,  false, true, "MC_CPU"},
    {24, false, true, "SUN"},
    {25,  false, false, "USB"},
    {26,  false, true, "AVD"},
    {27,  false, false, "Reserved"},
    {28,  false, false, "Reserved"},
    {29, false, false, "Reserved"},
    {30,  false, false, "Reserved"},
    {31, false, false, "Reserved"},
    {32, false, false, "EXT_IRQ0"},
    {33,  false, false, "EXT_IRQ1"},
    {34,  false, false, "EXT_IRQ2"},
    {35,  false, false, "EXT_IRQ3"},
    {36,  false, false, "EXT_IRQ4"},
    {37, false, false, "EXT_IRQ5"},
    {38, false, false, "EXT_IRQ6"},
    {39, false, false, "EXT_IRQ7"},
    {40, false, true, "IP0"},
    {41,  false, false, "IP1"},
    {42,  false, false, "USB_EHCI"},
    {43,  false, false, "USB_OHCI"},
    {44,  false, true, "XPT_PCR"},
    {45,  false, true, "XPT_FE"},
    {46,  false, false, "Reserved"},
    {47,  false, false, "Reserved"},
    {48,  false, false, "Reserved"},
    {49,  false, false, "Reserved"},
    {50,  false, false, "Reserved"},
    {51,  false, false, "Reserved"},
    {52,  false, false, "Reserved"},
    {53, false, false, "Reserved"}, /* reserved  */
    {54, false, false, "Reserved"},
    {55,  false, false, "Reserved"},
    {56,  false, false, "Reserved"},
    {57,  false, false, "Reserved"},
    {58, false, false,  "Reserved"},
    {59, false, false,  "Reserved"},
    {60,  false, false, "Reserved"},
    {61,  false, false, "Reserved"},
    {62,  false, false, "Reserved"},
    {63,  false, false, "Reserved"},
    {64,  false, false, "UPG_UART1"},
    {65,  false, false, "UPG_UART2"},
    {66, false, true,  "THD_CPU"},
    {67,  false, true, "DS0_CPU"},
    {68,  false, true, "DS1_CPU"},
    {69, false, true, "DS2_CPU"},
    {70,  false, false, "SPARE0"},
    {71, false, false, "SPARE1"},
    {72,  false, false, "SPARE2"},
    {73,  false, false, "SPARE3"},
    {74,  false, false, "SPARE4"},
    {75,  false, false, "SPARE5"},
    {76,  false, false, "SPARE6"},
    {77,  false, false, "SPARE7"},
    {78,  false, false, "Reserved"},
    {79,  false, false, "Reserved"},
    {80,  false, false, "Reserved"},
    {81,  false, false, "Reserved"},
    {82,  false, false, "Reserved"},
    {83,  false, false, "Reserved"},
    {84,  false, false, "Reserved"},
    {85,  false, false, "Reserved"},
    {86,  false, false, "Reserved"},
    {87,  false, false, "Reserved"},
    {88,  false, false, "Reserved"},
    {89,  false, false, "Reserved"},
    {90,  false, false, "Reserved"},
    {91,  false, false, "Reserved"},
    {92,  false, false, "Reserved"},
    {93,  false, false, "Reserved"},
    {94,  false, false, "Reserved"},
    {95,  false, false, "Reserved"},
    {0,   false, false, NULL}
}; /* End 7550 */
#elif BCHP_CHIP == 7358
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {1,  false, true, "AIO"},
    {2,  false, true, "BSP"},
    {3,  false, true, "BVNB"},
    {4,  false, true, "BVNF0"},
    {5,  false, true, "BVNF1"},
    {6,  false, true, "BVNF5"},
    {7,  false, true, "BVNM"},
    {8,  false, true, "CLKGEN"},
    {9,  false, false,"EXT_IRQ0"},
    {10, false, false,"EXT_IRQ1"},
    {11, false, false,"EXT_IRQ2"},
    {12, false, false,"EXT_IRQ3"},
    {13, false, false,"EXT_IRQ4"},
    {14, false, false,"EXT_IRQ5"},
    {15, false, false,"EXT_IRQ6"},
    {16, false, false,"EXT_IRQ7"},
    {17, false, false,"EXT_IRQ8"},
    {18, false, false,"EXT_IRQ9"},
    {19, false, false,"EXT_IRQ10" },
    {20, false, false,"EXT_IRQ11"},
    {21, false, false,"EXT_IRQ12"},
    {22, false, false,"EXT_IRQ13"},
    {23, false, false,"EXT_IRQ14"},
    {24, false, true, "FTM"},
    {25, false, false,"GENET_0_A"},
    {26, false, false,"GENET_0_B"},
    {27, false, false,"SPARE0"},
    {28, false, false,"SPARE1"},
    {29, false, true, "M2MC"},
    {30, false, true, "HDMI_TX"},
    {31, false, false,"HIF"},
    {32, false, false,"HIF_SPI"},
    {33, false, true, "IPI0"},
    {34, false, true, "IPI1"},
    {35, false, true, "MEMC0"},
    {36, false, false,"SPARE2"},
    {37, false, true, "NMI_PIN"},
    {38, false, true, "RAAGA"},
    {39, false, true, "RAAGA_FW"},
    {40, false, true, "SDS0_AFEC"},
    {41, false, true, "SDS0_RCVR_0"},
    {42, false, true, "SDS0_RCVR_1"},
    {43, false, true, "SDS0_TFEC"},
    {44, false, false,"SPARE3"},
    {45, false, false,"SPARE4"},
    {46, false, false,"SPARE5"},
    {47, false, false,"SPARE6"},
    {48, false, true, "SOFT_MODEM"},
    {49, false, true, "SYS"},
    {50, false, true, "SYS_AON"},
    {0, false, true, "SYS_PM"},
    {52, false, false,"SPARE7"},
    {53, false, false,"SPARE8"},
    {54, false, true, "UPG_AUX_AON"},
    {55, false, true, "UPG_BSC"},
    {56, false, true, "UPG_BSC_AON"},
    {57, false, true, "UPG_MAIN"},
    {58, false, true, "UPG_MAIN_AON"},
    {59, false, true, "UPG_SC"},
    {60, false, true, "UPG_SPI"},
    {61, false, true, "UPG_TMR"},
    {62, false, false,"UPG_UART0"},
    {63, false, false,"UPG_UART1"},
    {64, false, false,"UPG_UART2"},
    {65, false, false,"USB0_BRIDGE"},
    {66, false, false,"USB0_EHCI_0"},
    {67, false, false,"USB0_OHCI_0"},
    {68, false, false,"SPARE9"},
    {69, false, false,"SPARE10"},
    {70, false, false,"SPARE11"},
    {71, false, false,"SPARE12"},
    {72, false, false,"SPARE13"},
    {73, false, false,"SPARE14"},
    {74, false, false,"SPARE15"},
    {75, false, true, "VEC"},
    {76, false, true, "XPT_FE"},
    {77, false, true, "XPT_MSG"},
    {78, false, true, "XPT_MSG_STAT"},
    {79, false, true, "XPT_OVFL"},
    {80, false, true, "XPT_PCR"},
    {81, false, true, "XPT_RAV"},
    {82, false, true, "XPT_STATUS"},
    {83, false, false,"SDIO0"},
    {84, false, false,"SPARE16"},
    {85, false, true, "AVD0"},
    {86, false, false,"SPARE17"},
    {87, false, false,"SPARE18"},
    {88, false, false,"RFM"},
    {89, false, false,"THD_A"},
    {90, false, false,"THD_B"},
    {91, false, false,"UFE_CPU"},
    {92, false, false,"DS_0"},
    {93, false, false,"SPARE19"},
    {94, false, false,"SPARE20"},
    {95, false, false,"MCIF"},
    {96, false, true, "UPG_AUX"},
    {0,   false, false, NULL}
}; /* End 7358 */
#elif BCHP_CHIP == 7552
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[] = {
    {1,  false, true, "AIO"},
    {2,  false, true, "BSP"},
    {3,  false, true, "BVNB"},
    {4,  false, true, "BVNF0"},
    {5,  false, true, "BVNF1"},
    {6,  false, true, "BVNF5"},
    {7,  false, true, "BVNM"},
    {8,  false, true, "CLKGEN"},
    {9,  false, false,"EXT_IRQ0"},
    {10, false, false,"EXT_IRQ1"},
    {11, false, false,"EXT_IRQ2"},
    {12, false, false,"EXT_IRQ3"},
    {13, false, false,"EXT_IRQ4"},
    {14, false, false,"EXT_IRQ5"},
    {15, false, false,"EXT_IRQ6"},
    {16, false, false,"EXT_IRQ7"},
    {17, false, false,"EXT_IRQ8"},
    {18, false, false,"EXT_IRQ9"},
    {19, false, false,"EXT_IRQ10" },
    {20, false, false,"EXT_IRQ11"},
    {21, false, false,"EXT_IRQ12"},
    {22, false, false,"EXT_IRQ13"},
    {23, false, false,"EXT_IRQ14"},
    {24, false, false,"FTM"},
    {25, false, false,"GENET_0_A"},
    {26, false, false,"GENET_0_B"},
    {27, false, false,"SPARE0"},
    {28, false, false,"SPARE1"},
    {29, false, true, "M2MC"},
    {30, false, true, "HDMI_TX"},
    {31, false, false,"HIF"},
    {32, false, false,"HIF_SPI"},
    {33, false, true, "IPI0"},
    {34, false, true, "IPI1"},
    {35, false, true, "MEMC0"},
    {36, false, false,"SPARE2"},
    {37, false, true, "NMI_PIN"},
    {38, false, true, "RAAGA"},
    {39, false, true, "RAAGA_FW"},
    {40, false, false,"SDS0_AFEC"},
    {41, false, false,"SDS0_RCVR_0"},
    {42, false, false,"SDS0_RCVR_1"},
    {43, false, false,"SDS0_TFEC"},
    {44, false, false,"SPARE3"},
    {45, false, false,"SPARE4"},
    {46, false, false,"SPARE5"},
    {47, false, false,"SPARE6"},
    {48, false, true, "SOFT_MODEM"},
    {49, false, true, "SYS"},
    {50, false, true, "SYS_AON"},
    {0, false, true, "SYS_PM"},
    {52, false, false,"SPARE7"},
    {53, false, false,"SPARE8"},
    {54, false, true, "UPG_AUX_AON"},
    {55, false, true, "UPG_BSC"},
    {56, false, true, "UPG_BSC_AON"},
    {57, false, true, "UPG_MAIN"},
    {58, false, true, "UPG_MAIN_AON"},
    {59, false, true, "UPG_SC"},
    {60, false, true, "UPG_SPI"},
    {61, false, true, "UPG_TMR"},
    {62, false, false,"UPG_UART0"},
    {63, false, false,"UPG_UART1"},
    {64, false, false,"UPG_UART2"},
    {65, false, false,"USB0_BRIDGE"},
    {66, false, false,"USB0_EHCI_0"},
    {67, false, false,"USB0_OHCI_0"},
    {68, false, false,"SPARE9"},
    {69, false, false,"SPARE10"},
    {70, false, false,"SPARE11"},
    {71, false, false,"SPARE12"},
    {72, false, false,"SPARE13"},
    {73, false, false,"SPARE14"},
    {74, false, false,"SPARE15"},
    {75, false, true, "VEC"},
    {76, false, true, "XPT_FE"},
    {77, false, true, "XPT_MSG"},
    {78, false, true, "XPT_MSG_STAT"},
    {79, false, true, "XPT_OVFL"},
    {80, false, true, "XPT_PCR"},
    {81, false, true, "XPT_RAV"},
    {82, false, true, "XPT_STATUS"},
    {83, false, false,"SDIO0"},
    {84, false, false,"SPARE16"},
    {85, false, true, "AVD0"},
    {86, false, false,"SPARE17"},
    {87, false, false,"SPARE18"},
    {88, false, true, "RFM"},
    {89, false, true, "THD_A"},
    {90, false, true, "THD_B"},
    {91, false, true, "UFE_CPU"},
    {92, false, true, "DS_0"},
    {93, false, false,"SPARE19"},
    {94, false, false,"SPARE20"},
    {95, false, false,"MCIF"},
    {96, false, true, "UPG_AUX"},
    {0,   false, false, NULL}
}; /* End 7552 */
#elif BCHP_CHIP == 7405
/* Nexus will use BINT_P_IntMap for L1's that have L2's and
will use NEXUS_Platform_P_IrqMap for L1's without L2's. */
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[] = {
    {38, false, true, "EXT_IRQ_1"},
    {39, false, true, "EXT_IRQ_2"},
    {40, false, true, "EXT_IRQ_3"},
    {41, false, true, "EXT_IRQ_4"},
    {43,  false, true, "EXT_IRQ_5"},
    {44,  false, true, "EXT_IRQ_6"},
    {45,  false, true, "EXT_IRQ_7"},
    {46,  false, true, "EXT_IRQ_8"},
    {47,  false, true, "EXT_IRQ_9"},
    {48,  false, true, "EXT_IRQ_10"},
    {49,  false, true, "EXT_IRQ_11"},
    {50,  false, true, "EXT_IRQ_12"},
    {51,  false, true, "EXT_IRQ_13"},
    {52,  false, true, "EXT_IRQ_14"},
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
#if BCHP_VER >= BCHP_VER_B0
    {12, false, true, "EXT_IRQ0"},
    {13, false, true, "EXT_IRQ1"},
    {14, false, true, "EXT_IRQ2"},
    {15, false, true, "EXT_IRQ3"},
    {16, false, true, "EXT_IRQ4"},
    {75, false, true, "V3D"},
#else
    {11, false, true, "EXT_IRQ0"},
    {12, false, true, "EXT_IRQ1"},
    {13, false, true, "EXT_IRQ2"},
    {14, false, true, "EXT_IRQ3"},
    {15, false, true, "EXT_IRQ4"},
#endif
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7231)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[] = {
    {9,  false, true, "EXT_IRQ0"},
    {10, false, true, "EXT_IRQ1"},
    {11, false, true, "EXT_IRQ2"},
    {12, false, true, "EXT_IRQ3"},
    {13, false, true, "EXT_IRQ4"},
    {0,  false, false, NULL}
};
#elif (BCHP_CHIP==7429)
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {10,  false, true, "EXT_IRQ0"},
    {11,  false, true, "EXT_IRQ1"},
    {12,  false, true, "EXT_IRQ2"},
    {13,  false, true, "EXT_IRQ3"},
    {14,  false, true, "EXT_IRQ4"},
    {15,  false, true, "EXT_IRQ5"},
    {0,   false, false, NULL}
};
#elif (BCHP_CHIP==7435)
static void upg_cpu_handler(int irq);
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[32*NEXUS_NUM_L1_REGISTERS] = {
    {12, false, true, "EXT_IRQ0"},
    {13, false, true, "EXT_IRQ1"},
    {14, false, true, "EXT_IRQ2"},
    {15, false, true, "EXT_IRQ3"},
    {16, false, true, "EXT_IRQ4"},
    {80, false, true, "V3D"},
    {0,  false, false, NULL}
};
#else
/* in the default case, nexus will use BINT_P_IntMap for all L1 validation */
struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[] = {
    {0, false, false, NULL}
};
#endif

#if BCHP_CHIP == 7400
/*
    manage: kbd1irq(0), kbd2irqi(5), kbd3irq(8), kbd4irq(10), ldkirq(1), irbirq(2), gioirq(6), icapirq(7)
    don't manage: uairq(4), ubirq(3), ucirq(9), spi(20) iica..e(24..28).
*/
#define BCM7400_UPG_MANAGED_BITS (0x000005e7)
static void upg_cpu_handler(int irq)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0400780; /* UPG_CPU_INTR, shared with kernel */
    unsigned long status, enable, triggered_ints;

    status = l2_base[1];
    enable = l2_base[0];

    triggered_ints = status & enable & BCM7400_UPG_MANAGED_BITS;

    /* Mask all triggered interrupts */
    if (triggered_ints) {
        l2_base[0] = l2_base[0] & ~triggered_ints;
    }
}
#elif (BCHP_CHIP==7325)
/*
   manage: kbd1irq(0),ldkirq(1),irbirq(2),3,4,kbd2irq(5),gioirq(6),icapirq(7),kbd3irq(8)-->  UPG_CPU_INTR
   dont manage: uairq, ubirq, ucirq
   spiirq spiirq  -->  UPG_SPI_CPU_INTR
    iicairq, iicbirq, iiccirq, iicdirq  -->  UPG_BSC_CPU_INTR */
#define BCM7325_UPG_MANAGED_BITS (0x000003e7)
static void upg_cpu_handler(int irq)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0400780; /* UPG_CPU_INTR, shared with kernel */
    unsigned long status, enable, triggered_ints;

    status = l2_base[1];
    enable = l2_base[0];

    triggered_ints = status & enable & BCM7325_UPG_MANAGED_BITS;

    /* Mask all triggered interrupts */
    if (triggered_ints) {
        l2_base[0] = l2_base[0] & ~triggered_ints;
    }
}
#elif BCHP_CHIP == 7420
/*
   manage: kbd1irq(0),ldkirq(1),irbirq(2),3,4,kbd2irq(5),gioirq(6),icapirq(7),kbd3irq(8)-->  UPG_CPU_INTR
   dont manage: uairq, ubirq, ucirq
   spiirq spiirq  -->  UPG_SPI_CPU_INTR
    iicairq, iicbirq, iiccirq, iicdirq  -->  UPG_BSC_CPU_INTR */
#define BCM7420_UPG_MANAGED_BITS (0x000001e7)
static void upg_cpu_handler(int irq)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0406780; /* UPG_CPU_INTR, shared with kernel */
    unsigned long status, enable, triggered_ints;

    status = l2_base[1];
    enable = l2_base[0];

    triggered_ints = status & enable & BCM7420_UPG_MANAGED_BITS;

    /* Mask all triggered interrupts */
    if (triggered_ints) {
        l2_base[0] = l2_base[0] & ~triggered_ints;
    }
}

/*
 * These are only L2 bits for L1 27 interrupt handled by the bint_isr for
 *  both user and kernel mode driver
 */

#define BCM7420_MCIF_MANAGED_BITS (0x00380000)
#define BCM7420_CTK_MANAGED_BITS (0x1ff) /* bits 0 through 8 */

void mcif_cpu_handler(int irq)
{
    unsigned long mcif_triggeredInts=0;
    unsigned long ctk_triggeredInts=0;
    volatile unsigned long *mcif_l2_status = (volatile unsigned long *)0xB0407080;
    volatile unsigned long *mcif_l2_mask = (volatile unsigned long *)0xB0407090;
    volatile unsigned long *ctk_l2_status = (volatile unsigned long *)0xB04073c0;
    volatile unsigned long *ctk_l2_mask = (volatile unsigned long *)0xB04073d0;

    mcif_triggeredInts = *mcif_l2_status & BCM7420_MCIF_MANAGED_BITS;
    if(mcif_triggeredInts)
    {
        *mcif_l2_mask |= mcif_triggeredInts;

    }

    ctk_triggeredInts = *ctk_l2_status &  BCM7420_CTK_MANAGED_BITS;

    if(ctk_triggeredInts)
    {
       *ctk_l2_mask = ctk_triggeredInts;
    }

    /*
     * Interrupts will be unmasked in BINT_isr() based on the L2 Invalid
     * Mask parameter. The returned parameter *triggeredInts should be positive
     * in case some interrupt was triggered to take appropriate action later
     * in the bcmdriver.
     */
     return;
}
#endif
