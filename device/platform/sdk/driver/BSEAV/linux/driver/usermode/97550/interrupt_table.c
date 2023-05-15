/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: interrupt_table.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 3/17/11 1:51p $
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/97550/interrupt_table.c $
 * 
 * 8   3/17/11 1:51p nitinb
 * SW7550-708: Removed BCHP_VER
 * 
 * 7   2/16/11 6:15p nitinb
 * SW7550-670: Enabled SHARF interrupt for 7550 B0
 * 
 * 6   1/28/11 3:26p nitinb
 * SW7550-667: Enable M2MC interrupt for 7550 B0
 * 
 * 5   6/16/10 6:21p nitinb
 * SW7550-454: Enable interrupt bits for UART-B and UART-C
 * 
 * 4   11/13/09 10:28a nitinb
 * SW7550-68: Removing the mis-match between bint_7550.c and
 * interrupt_table.c
 * 
 * 3   11/9/09 5:00p nitinb
 * SW7550-46: Corrected the interrupt register address in g_sChipConfig.
 * 
 * 2   10/6/09 4:27p nitinb
 * SW7550-46: Fixing compilation errors
 * 
 * 1   9/28/09 11:37a nitinb
 * SW7550-46: Add Linux user mode driver support for 7550
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
#define BCM7550_UPG_MANAGED_BITS (0x0000028F)

void bcm7550_upg_l2share_func(int irq, unsigned long *triggeredInts)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0406780; /* UPG_CPU_INTR, shared with kernel */
    unsigned long status, enable;
    
    status = l2_base[1];
    enable = l2_base[0];

    *triggeredInts = status & enable & BCM7550_UPG_MANAGED_BITS;

    /* Mask all triggered interrupts */
    if (*triggeredInts) {
        l2_base[0] = l2_base[0] & ~(*triggeredInts);
    }
}

/* function to mask the managed interrupts */
void bcm7550_upg_l2mask_func(int irq, unsigned long *unused)
{
    volatile unsigned long *l2_base = (volatile unsigned long *)0xB0406780; /* UPG_CPU_INTR, shared with kernel */
    /* Mask all managed interrupts */
    l2_base[0] = l2_base[0] & ~BCM7550_UPG_MANAGED_BITS;
    
}

s_InteruptTable interruptName7550[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"HIF_CPU_INTR",             0},/* 0 */
    {"HIF_SPI_CPU_INTR",         INT_ENABLE_MASK},/* 1 */
    {"XPT_STATUS_CPU_INTR",      INT_ENABLE_MASK},/* 2 */
    {"XPT_RAV_CPU_INTR",         INT_ENABLE_MASK},/* 3 */
    {"XPT_ICAM_CPU_INTR",        INT_ENABLE_MASK},/* 4 */
    {"AIO_CPU_INTR",             INT_ENABLE_MASK},/* 5 */
    {"RPTD_CPU_INTR",            INT_ENABLE_MASK},/* 6 */
    {"DVP_HT_CPU_INTR",          INT_ENABLE_MASK},/* 7 */
    {"VEC_CPU_INTR",             INT_ENABLE_MASK},/* 8 */
    {"BVNB_CPU_INTR",            INT_ENABLE_MASK},/* 9 */
    {"BVNF_CPU_INTR_0",          INT_ENABLE_MASK},/* 10 */
    {"BVNF_CPU_INTR_1",          INT_ENABLE_MASK},/* 11 */
    {"BVNF_CPU_INTR_5",          INT_ENABLE_MASK},/* 12 */
    {"MEMC_0_CPU_INTR",          INT_ENABLE_MASK},/* 13 */
    {"ENET_EMAC1_CPU_INTR",      0},/* 14 */
    {"UPG_CPU_INTR",             INT_ENABLE_MASK, 0, bcm7550_upg_l2share_func, bcm7550_upg_l2mask_func},/* 15 */
    {"UPG_GPIO_CPU_INTR",        INT_ENABLE_MASK},/* 16 */
    {"UPG_TMR_CPU_INTR",         INT_ENABLE_MASK},/* 17 */
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 18 */
    {"UPG_SPI_CPU_INTR",         INT_ENABLE_MASK}, /* 19 */
    {"UPG_UART0_CPU_INTR",       0}, /* 20 */
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 21 */
    {"MC_CPU_INTR",              INT_ENABLE_MASK}, /* 22 */
    {"SUN_CPU_INTR",             INT_ENABLE_MASK}, /* 23 */
    {"USB_CPU_INTR",             0}, /* 24 */
    {"AVD0_CPU_INTR",            INT_ENABLE_MASK}, /* 25 */
    {NULL,                       0}, /* 26 */
    {NULL,                       0}, /* 27 */
    {NULL,                       0}, /* 28 */
    {NULL,                       0}, /* 29 */
    {NULL,                       0}, /* 30 */
    {NULL,                       0}, /* 31 */
    {"EXT_IRQ_0_CPU_INTR",       0 | INT_REG_WRAP}, /* 0 */
    {"EXT_IRQ_1_CPU_INTR",       0}, /* 1 */
    {"EXT_IRQ_2_CPU_INTR",       0}, /* 2 */
    {"EXT_IRQ_3_CPU_INTR",       0}, /* 3 */
    {"EXT_IRQ_4_CPU_INTR",       0}, /* 4 */
    {"EXT_IRQ_5_CPU_INTR",       0}, /* 5 */
    {"EXT_IRQ_6_CPU_INTR",       0}, /* 6 */
    {"EXT_IRQ_7_CPU_INTR",       0}, /* 7 */
    {"IPI0_CPU_INTR",            0}, /* 8, */
    {"IPI1_CPU_INTR",            0}, /* 9 */
    {"USB_EHCI_CPU_INTR",        0}, /* 10 */
    {"USB_OHCI_CPU_INTR",        0}, /* 11 */
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 12 */
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 13 */
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
    {NULL,                       0}, /* 31 */
    {"UPG_UART1_CPU_INTR",       0 | INT_REG_WRAP}, /* 0 */
    {"UPG_UART2_CPU_INTR",       0}, /* 1 */
    {"THD_CPU_INTR",             INT_ENABLE_MASK}, /* 2 */
    {"DS0_CPU_INTR_0",           INT_ENABLE_MASK}, /* 3 */
    {"DS1_CPU_INTR_1",           INT_ENABLE_MASK}, /* 4 */
    {"DS2_CPU_INTR_2",           INT_ENABLE_MASK}, /* 5 */
    {"M2MC_CPU_INTR",            INT_ENABLE_MASK}, /* 6 */
    {"SHARF_CPU_INTR",           INT_ENABLE_MASK}, /* 7 */
    {"SPARE_2_CPU_INTR",         0}, /* 8 */
    {"SPARE_3_CPU_INTR",         0}, /* 9 */
    {"SPARE_4_CPU_INTR",         0}, /* 10 */
    {"SPARE_5_CPU_INTR",         0}, /* 11 */
    {"SPARE_6_CPU_INTR",         0}, /* 12 */
    {"SPARE_7_CPU_INTR",         0}, /* 13 */
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


s_ChipConfig  g_sChipConfig ={ "7550",interruptName7550, 0xb0001400, sizeof(interruptName7550)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
