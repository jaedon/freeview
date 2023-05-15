/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: interrupt_table.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 11/4/10 5:03p $
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/97335/interrupt_table.c $
 * 
 * 10   11/4/10 5:03p jrubio
 * SW7335-627: fix 7335 interrupt table, remove HIF add NMI
 * 
 * 9   11/19/09 1:26p jrubio
 * SW7335-627: add 2631-1.0 support
 * 
 * 8   9/18/09 2:43p jrubio
 * SW7335-549: make sure uc_irqen is enabled and not masked
 * 
 * 7   8/7/08 12:29p katrep
 * PR45473: removed chip dependancy for the common file bcm_driver.c
 * 
 * 6   5/14/08 8:00p jrubio
 * PR42692: correct UPG address
 * 
 * 5   5/8/08 9:49a jrubio
 * PR42353: enable MEMC1 interrupt
 * 
 * 4   5/2/08 1:55p jrubio
 * PR42353: update 7335 config , enable MEMC1 interrupt
 * 
 * 3   12/11/07 2:21p katrep
 * PR37217: Updated the interrupt table.
 * 
 * 2   11/21/07 3:43p katrep
 * PR37217: Added 97335 platform support.
 * 
 * 1   11/21/07 1:19p katrep
 * PR37430: Added suppport for upto 128 bit interrupt
 * controller,reoganized the driver code.
 *
 ***************************************************************************/
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include "bcmdriver_common.h"

#define NUM_INTC_WORDS   3   /* Number of interrupt controller words */

 /*
manage: kbd1irq(0),ldkirq(1),irbirq(2),3,4,kbd2irq(5),gioirq(6),icapirq(7),kbd3irq(8)-->  UPG_CPU_INTR
dont manage: uairq, ubirq, ucirq
spiirq spiirq  -->  UPG_SPI_CPU_INTR
iicairq, iicbirq, iiccirq, iicdirq  -->  UPG_BSC_CPU_INTR */
#define BCM7335_UPG_MANAGED_BITS (0x000003e7)

void bcm7335_upg_l2share_func(int irq, unsigned long *triggeredInts)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB04006C0; /* UPG_CPU_INTR, shared with kernel */
    unsigned long status, enable;
    
    status = l2_base[1];
    enable = l2_base[0];


    *triggeredInts = status & enable & BCM7335_UPG_MANAGED_BITS;

    /* Mask all triggered interrupts */
    if (*triggeredInts) {
        l2_base[0] = l2_base[0] & ~(*triggeredInts);
    }
}

/* function to mask the managed interrupts */
void bcm7335_upg_l2mask_func(int irq, unsigned long *unused)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB04006C0; /* UPG_CPU_INTR, shared with kernel */
    /* Mask all managed interrupts */
    l2_base[0] = l2_base[0] & ~BCM7335_UPG_MANAGED_BITS;
    
}

s_InteruptTable interruptName7335[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"HIF_CPU_INTR",             0},/* 0 */
    {"XPT_STATUS_CPU_INTR",      INT_ENABLE_MASK},/* 1 */
    {"XPT_OVFL_CPU_INTR",        INT_ENABLE_MASK},/* 2 */
    {"XPT_MSG_CPU_INTR",         INT_ENABLE_MASK},/* 3 */
    {"XPT_ICAM_CPU_INTR",        INT_ENABLE_MASK},/* 4 */
    {"BSP_CPU_INTR",             INT_ENABLE_MASK},/* 5 */
    {"AIO_CPU_INTR",             INT_ENABLE_MASK},/* 6 */
    {"GFX_CPU_INTR",             INT_ENABLE_MASK},/* 7 */
    {"HDMI_CPU_INTR",            INT_ENABLE_MASK},/* 8 */
    {"RPTD_CPU_INTR",            INT_ENABLE_MASK},/* 9 */
    {"VEC_CPU_INTR",             INT_ENABLE_MASK},/* 10 */
    {"BVNB_CPU_INTR",            INT_ENABLE_MASK},/* 11 */
    {"BVNF_CPU_INTR_0",          INT_ENABLE_MASK},/* 12 */
    {"BVNF_CPU_INTR_1",          INT_ENABLE_MASK},/* 13 */
    {"MEMC_0_CPU_INTR",          INT_ENABLE_MASK},/* 14 */
    {"ENET_CPU_INTR",            0},/* 15 */
    {"RFM_CPU_INTR",             INT_ENABLE_MASK},/* 16 */
    {"UPG_TMR_CPU_INTR",         INT_ENABLE_MASK},/* 17 */
    {"UPG_CPU_INTR",             INT_ENABLE_MASK}, /* 18 */
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 19 */
    {"UPG_SPI_CPU_INTR",         INT_ENABLE_MASK}, /* 20 */
    {"UPG_UART0_CPU_INTR",       0}, /* 21 */
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 22 */
    {"SUN_CPU_INTR",             INT_ENABLE_MASK}, /* 23 */
    {"UHF1_CPU_INTR",            INT_ENABLE_MASK}, /* 24 */
    {"SDS_RCVR_1_CPU_INTR",      INT_ENABLE_MASK}, /* 25 */
    {"USB_CPU_INTR",             0}, /* 26 */
    {"MC_CPU_INTR",              0}, /* 27 */
    {"BVNF_CPU_INTR_3",          INT_ENABLE_MASK}, /* 28 */
    {"SATA_PCIB_CPU_INTR",       0}, /* 29 */
    {"AVD0_CPU_INTR_0",          INT_ENABLE_MASK}, /* 30 */
    {"XPT_RAVE_CPU_INTR",        INT_ENABLE_MASK}, /* 31 */
    {"PCI_INTA_0_CPU_INTR",      0 | INT_REG_WRAP}, /* 0 */
    {"PCI_INTA_1_CPU_INTR",      0}, /* 1 */
    {"PCI_INTA_2_CPU_INTR",      0}, /* 2 */
    {"SDS1_RCVR_0_CPU_INTR",     INT_ENABLE_MASK}, /* 3 */
    {"EXT_IRQ_0_CPU_INTR",       INT_ENABLE_MASK}, /* 4 */
    {"EXT_IRQ_1_CPU_INTR",       0}, /* 5 */
    {"EXT_IRQ_2_CPU_INTR",       0}, /* 6, SLOT 2 */
    {"EXT_IRQ_3_CPU_INTR",       0}, /* 7 */
    {"EXT_IRQ_4_CPU_INTR",       0}, /* 8, SLOT 0 */
    {"PCI_SATA_PCI_INTR",        0}, /* 9 */
    {"EXT_IRQ_5_CPU_INTR",       0}, /* 10 */
    {"EXT_IRQ_6_CPU_INTR",       0}, /* 11 */
    {"EXT_IRQ_7_CPU_INTR",       0}, /* 12, SLOT 2 */
    {"EXT_IRQ_8_CPU_INTR",       0}, /* 13 */
    {"EXT_IRQ_9_CPU_INTR",       0}, /* 14 */
    {"EXT_IRQ_10_CPU_INTR",      0}, /* 15 */
    {"EXT_IRQ_11_CPU_INTR",      0}, /* 16 */
    {"EXT_IRQ_12_CPU_INTR",      0}, /* 17 */
    {"EXT_IRQ_13_CPU_INTR",      0}, /* 18, 3517 */
    {"EXT_IRQ_14_CPU_INTR",      0}, /* 19, 4500 */
    {"IPI0_CPI_INTR",            INT_ENABLE_MASK}, /* 20 */
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 21 */
    {"SDS0_RCVR_0_CPU_INTR",     INT_ENABLE_MASK}, /* 22 */
    {"BVNF_CPU_INTR_6",          INT_ENABLE_MASK}, /* 23 */
    {"USB_EHCI_1_CPU_INTR",      0}, /* 24 */
    {"SM_CPU_INTR",              0}, /* 25 */
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 26 */
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 27 */
    {"XPT_MSG_STAT_CPU_INTR",    INT_ENABLE_MASK}, /* 28 */
    {"USB_EHCI_CPU_INTR",        0}, /* 29 */
    {"USB_OHCI_0_CPU_INTR",      0}, /* 30 */
    {"USB_OHCI_1_CPU_INTR",      0},  /* 31 */
    {"ENET_EMAC1_CPU_INTR",      0 | INT_REG_WRAP}, /* 0 */
    {"UPG_UART1_CPU_INTR",       0}, /* 1 */
    {"UPG_UART2_CPU_INTR",       0}, /* 2 */
    {"SDS1_RCVR_1_CPU_INTR",     INT_ENABLE_MASK}, /* 3 */
    {"QPSK_RCVR_0_CPU_INTR",     INT_ENABLE_MASK}, /* 4 */
    {"QPSK_RCVR_1_CPU_INTR",     INT_ENABLE_MASK}, /* 5 */
    {"AFEC_CORE_0_CPU_INTR",     INT_ENABLE_MASK}, /* 6 */
    {"AFEC_CORE_1_CPU_INTR",     INT_ENABLE_MASK}, /* 7 */
    {"MEMC_1_CPU_INTR",         INT_ENABLE_MASK}, /* 8 */
    {"FTM_CPU_INTR",             INT_ENABLE_MASK}, /* 9 */
    {"NMI_CPU_INTR",             INT_ENABLE_MASK}, /* 10 */
    {NULL,                       0}, /* 11 */
    {NULL,                       0}, /* 12 */
    {NULL,                       0}, /* 13 */
    {NULL,                       0}, /* 14 */
    {NULL,                       0}, /* 15 */
    {NULL,                       0}, /* 16 */
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


s_ChipConfig  g_sChipConfig ={ "7335",interruptName7335, 0xb0001400, sizeof(interruptName7335)/sizeof(s_InteruptTable),NUM_INTC_WORDS};