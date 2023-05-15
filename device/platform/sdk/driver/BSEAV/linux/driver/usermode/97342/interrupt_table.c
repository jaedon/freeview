/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
 *
 * $brcm_Workfile: interrupt_table.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 9/29/10 4:33p $
 *
 * Module Description:
 * This file contains the platform specific interrupt 
 *   tables and shread functions 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/97342/interrupt_table.c $
 * 
 * 7   9/29/10 4:33p jrubio
 * SW7342-121: enable interrupts MC
 * 
 * 6   6/24/10 2:31p jrubio
 * SW7342-121: enable the MC interrupt
 * 
 * 5   12/2/09 5:34p jrubio
 * SW7342-121: disable UPG L2 Sharing and cleanup table
 * 
 * 4   8/12/09 10:18p jrubio
 * PR55232: disable HIF
 * 
 * 3   8/12/09 7:45p jrubio
 * PR55232: update table
 * 
 * 2   8/4/09 5:19p jrubio
 * PR55232: remove warning
 * 
 * 1   8/4/09 5:17p jrubio
 * PR55232: add 7342 table
 * 
 *
 *
 ***************************************************************************/
#include <linux/autoconf.h>
#include <linux/module.h>
#include "bcmdriver_common.h"

#define NUM_INTC_WORDS   3   /* Number of interrupt controller words */

 /*
manage: kbd1irq(0),ldkirq(1),irbirq(2),3,4,kbd2irq(5),gioirq(6),icapirq(7),kbd3irq(8)-->  UPG_CPU_INTR
dont manage: uairq, ubirq, ucirq
spiirq spiirq  -->  UPG_SPI_CPU_INTR
iicairq, iicbirq, iiccirq, iicdirq  -->  UPG_BSC_CPU_INTR */
#define BCM7342_UPG_MANAGED_BITS (0x000001e7)

void bcm7342_upg_l2share_func(int irq, unsigned long *triggeredInts)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0406780; /* UPG_CPU_INTR, shared with kernel */
    unsigned long status, enable;
    
    status = l2_base[1];
    enable = l2_base[0];

    *triggeredInts = status & enable & BCM7342_UPG_MANAGED_BITS;

    /* Mask all triggered interrupts */
    if (*triggeredInts) {
        l2_base[0] = l2_base[0] & ~(*triggeredInts);
    }
}

/* function to mask the managed interrupts */
void bcm7342_upg_l2mask_func(int irq, unsigned long *unused)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0406780; /* UPG_CPU_INTR, shared with kernel */
    /* Mask all managed interrupts */
    l2_base[0] = l2_base[0] & ~BCM7342_UPG_MANAGED_BITS;
    
}

s_InteruptTable interruptName7342[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"HIF_CPU_INTR",            0},/* 0 (1)*/
    {"XPT_STATUS_CPU_INTR",      INT_ENABLE_MASK},/* 1  (2)*/
    {"XPT_OVFL_CPU_INTR",        INT_ENABLE_MASK},/* 2 (3) */
    {"XPT_MSG_CPU_INTR",         INT_ENABLE_MASK},/* 3 (4) */
    {"XPT_ICAM_CPU_INTR",        INT_ENABLE_MASK},/* 4 (5) */
    {"BSP_CPU_INTR",             INT_ENABLE_MASK},/* 5 (6) */
    {"AIO_CPU_INTR",             INT_ENABLE_MASK},/* 6 (7)*/
    {"GFX_CPU_INTR",             INT_ENABLE_MASK},/* 7 (8)*/
    {"HDMI_CPU_INTR",            INT_ENABLE_MASK},/* 8 (9)*/
    {"RPTD_CPU_INTR",            INT_ENABLE_MASK},/* 9 (10)*/
    {"VEC_CPU_INTR",             INT_ENABLE_MASK},/* 10 (11)*/
    {"BVNB_CPU_INTR",            INT_ENABLE_MASK},/* 11 (12)*/
    {"BVNF_CPU_INTR_0",          INT_ENABLE_MASK},/* 12 (13)*/
    {"BVNF_CPU_INTR_1",          INT_ENABLE_MASK},/* 13 (14)*/
    {"MEMC_0_CPU_INTR",          INT_ENABLE_MASK},/* 14 (15)*/
    {"GENET_0_A_CPU_INTR",            0},/* 15 (16)*/
    {"PM_CPU_INTR",            INT_ENABLE_MASK},/* 16 (17)*/
    {"FTM_CPU_INTR",         INT_ENABLE_MASK},/* 17 (18)*/
    {"UPG_TMR_CPU_INTR",             INT_ENABLE_MASK}, /* 18 (19)*/
    {"UPG_CPU_INTR",         INT_ENABLE_MASK}, /* 19 (20)*/
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 20 (21) */
    {"UPG_SPI_CPU_INTR",       INT_ENABLE_MASK}, /* 21 (22)*/
    {"UPG_UART0_CPU_INTR",    0}, /* 22 (23)*/
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 23 (24)*/
    {"SUN_CPU_INTR",             INT_ENABLE_MASK}, /* 24 (25)*/
    {"SDS0_RCVF_1_CPU_INTR",            INT_ENABLE_MASK}, /* 25 (26) */
    {"UHF1_CPU_INTR",           INT_ENABLE_MASK}, /* 26 (27)*/
    {"USB_CPU_INTR",             0}, /* 27 (28)*/
    {"MC_CPU_INTR",               INT_ENABLE_MASK }, /* 28 (29) */
    {"BVNF_CPU_INTR_5",          INT_ENABLE_MASK}, /* 29 (30) */
    {"SATA_PCIB_CPU_INTR",       0}, /* 30 (31)*/
    {"AVD0_CPU_INTR",            INT_ENABLE_MASK}, /* 31 (32)*/
    {"PCI_INTA_0_CPU_INTR",      0 | INT_REG_WRAP}, /* 0 (33)*/
    {"PCI_INTA_1_CPU_INTR",      0}, /* 1 (34)*/
    {"PCI_INTA_2_CPU_INTR",      0}, /* 2 (35)*/
    {"SDS1_RCVR_0_CPU_INTR",            INT_ENABLE_MASK}, /* 3 (36)*/
    {"EXT_IRQ_0_CPU_INTR",       INT_ENABLE_MASK}, /* 4 (37)*/
    {"EXT_IRQ_1_CPU_INTR",       0}, /* 5 (38)*/
    {"EXT_IRQ_2_CPU_INTR",       0}, /*  6  (39)*/
    {"EXT_IRQ_3_CPU_INTR",       0 }, /* 7 (40)*/
    {"EXT_IRQ_4_CPU_INTR",       0 }, /* 8 (41) */
    {"PCI_SATA_CPU_INTR",        0}, /* 9 (42)*/
    {"EXT_IRQ_5_CPU_INTR",       0 }, /* 10 (43)*/
    {"EXT_IRQ_6_CPU_INTR",       0}, /* 11 (44)*/
    {"EXT_IRQ_7_CPU_INTR",       0 }, /* 12 (45) */
    {"EXT_IRQ_8_CPU_INTR",       0}, /* 13 (46)*/
    {"EXT_IRQ_9_CPU_INTR",       0}, /* 14 (47)*/
    {"EXT_IRQ_10_CPU_INTR",      INT_ENABLE_MASK}, /* 15 (48)*/
    {"EXT_IRQ_11_CPU_INTR",      INT_ENABLE_MASK}, /* 16 (49)*/
    {"EXT_IRQ_12_CPU_INTR",      INT_ENABLE_MASK}, /* 17 (50)*/
    {"EXT_IRQ_13_CPU_INTR",      INT_ENABLE_MASK}, /* 18 (51)*/
    {"EXT_IRQ_14_CPU_INTR",      INT_ENABLE_MASK}, /* 19 (52)*/
    {"IPI0_CPU_INTR",            INT_ENABLE_MASK}, /* 20 (53)*/
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 21 (54)*/
    {"XPT_MSG_STAT_CPU_INTR",            INT_ENABLE_MASK}, /* 22 (55)*/
    {"SDS0_RCVR_0_CPU_INTR",      INT_ENABLE_MASK}, /* 23 (56)*/
    {"SDS1_RCVR_1_CPU_INTR",      INT_ENABLE_MASK}, /* 24 (57)*/
    {"USB_EHCI_1_CPU_INTR",      0}, /* 25 (58)*/
    {"USB_OHCI_1_CPU_INTR",      0}, /* 26 (59)*/
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 27 (60)*/
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 28 (61)*/
    {"XPT_RAV_CPU_INTR",    INT_ENABLE_MASK}, /* 29 (62)*/
    {"USB_EHCI_0_CPU_INTR",      0}, /* 30 (63)*/
    {"USB_OHCI_0_CPU_INTR",      0}, /* 31 (64)*/
    {"SM_CPU_INTR",       0 | INT_REG_WRAP}, /* 0 (65)*/
    {"AFEC_CORE_0_CPU_INTR",       INT_ENABLE_MASK}, /* 1 (66)*/
    {"UPG_UART1_CPU_INTR",       0 }, /* 2 (67)*/
    {"UPG_UART2_CPU_INTR",       0}, /* 3 (68)*/
    {"NMI_PIN_CPU_INTR",        INT_ENABLE_MASK}, /* 4 (69)*/
    {"QPSK_RCVR_0_CPU_INTR",        INT_ENABLE_MASK}, /* 5 (70)*/
    {"QPSK_RCVR_1_CPU_INTR",        INT_ENABLE_MASK}, /* 6 (71)*/
    {"MOCA_INTR_CPU_INTR",       0}, /* 7 (72)*/
    {"MOCA_GENET_0_A_INTR_CPU_INTR",  0},  /* 8 (73)*/
    {"AFEC_CORE_1_CPU_INTR",  INT_ENABLE_MASK}, /* 9 (74)*/
    {"GENET_0_B_CPU_INTR",         0}, /* 10 (75)*/
    {"MOCA_GENET_0_B_INTR_CPU_INTR",  0},  /* 11 (76)*/
    {"BVNF_CPU_INTR_6",  INT_ENABLE_MASK}, /* 12 (77)*/
    {"USB_EHCI_2_CPU_INTR",       0}, /* 13 (78)*/
    {"USB_OHCI_2_CPU_INTR",      0}, /* 14 (79)*/
    {"SDS0_TFEC_CPU_INTR",     INT_ENABLE_MASK }, /* 15 (80)*/
    {"SDS1_TFEC_CPU_INTR",     INT_ENABLE_MASK }, /* 16 (81)*/
    {NULL,                       0}, /* 17 */
    {NULL,                       0}, /* 18 */
    {NULL,                       0}, /* 19 */
    {NULL,                       0}, /* 20 */
    {NULL,                       0}, /* 21 */
    {NULL,                       0}, /* 22 */
    {NULL,                       0}, /* 23 */
    {NULL,                       0}, /* 24 */
    {NULL,                       0}, /* 25 */
    {NULL,                       0}, /* 26 */
    {NULL,                       0}, /* 27 */
    {NULL,                       0}, /* 28 */
    {NULL,                       0}, /* 29 */
    {NULL,                       0}, /* 30 */
    {NULL,                       0}  /* 31 */
};


s_ChipConfig  g_sChipConfig ={ "7342",interruptName7342, 0xb0411400, sizeof(interruptName7342)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
