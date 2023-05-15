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
 * $brcm_Revision: 11 $
 * $brcm_Date: 11/1/11 4:10p $
 *
 * Module Description:
 * This file contains the platform specific interrupt tables
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/97405/interrupt_table.c $
 * 
 * 11   11/1/11 4:10p erickson
 * SW7425-365: add 2.6.37 support
 * 
 * 10   10/25/11 1:55p katrep
 * SW7405-5229:cleaned up compiler warning
 * 
 * 9   4/7/11 7:50p katrep
 * SW7405-5229:add support multipe heaps
 * 
 * 8   11/17/10 7:17p VISHK
 * SW7422-71: Add support for GPIO interrupts to 3112 frontend tuners
 * 
 * 7   9/18/09 11:47a erickson
 * SW7335-549: remove code for shared UPG_CPU interrupt. OS has dedicated
 * L1's for UART. soft modem has its own L2.
 *
 * 6   8/19/09 2:34p ssood
 * PR55232: add support for 2.6.3x kernel
 *
 * 5   3/3/09 1:06p vishk
 * PR 46431: ADS PI, Add support for BCM3117, BCM3114, and BCM3112.
 *
 * 4   12/30/08 5:55p katrep
 * PR50472: Interrupt support for P3D L2 for 7405 co and newer verison of
 * chips
 *
 * 3   8/7/08 12:29p katrep
 * PR45473: removed chip dependancy for the common file bcm_driver.c
 *
 * 2   2/29/08 5:04p vishk
 * PR 40148: 3510-B2 configuration for 97405 B0 platform
 *
 * 1   11/21/07 1:19p katrep
 * PR37430: Added suppport for upto 128 bit interrupt
 * controller,reoganized the driver code.
 *
 ***************************************************************************/
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37) 
#elif ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30) ) && ( LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37) )
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include "bcmdriver_common.h"

#define NUM_INTC_WORDS   3   /* Number of interrupt controller words */

s_InteruptTable interruptName7405[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"HIF_CPU_INTR",             INT_ENABLE_MASK},/* 0 */
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
    {"BVNF_CPU_INTR_2",          INT_ENABLE_MASK},/* 14 */
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
    {"PX3D_CPU_INTR",            INT_ENABLE_MASK}, /* 25 */
    {"USB_CPU_INTR",             0}, /* 26 */
    {"MC_CPU_INTR",              0}, /* 27 */
    {"BVNF_CPU_INTR_3",          INT_ENABLE_MASK}, /* 28 */
    {"SATA_PCIB_CPU_INTR",       0}, /* 29 */
    {"AVD0_CPU_INTR_0",          INT_ENABLE_MASK}, /* 30 */
    {"XPT_RAVE_CPU_INTR",        INT_ENABLE_MASK}, /* 31 */
    {"PCI_INTA_0_CPU_INTR",      INT_ENABLE_MASK | INT_REG_WRAP}, /* 0 */
    {"PCI_INTA_1_CPU_INTR",      INT_ENABLE_MASK}, /* 1 */
    {"PCI_INTA_2_CPU_INTR",      0}, /* 2 */
    {NULL,                       0}, /* 3 */
    {"EXT_IRQ_0_CPU_INTR",       INT_ENABLE_MASK}, /* 4 */
    {"EXT_IRQ_1_CPU_INTR",       INT_ENABLE_MASK}, /* 5, If the frontend has GPIO interrupts, this needs to be disabled. */
    {"EXT_IRQ_2_CPU_INTR",       INT_ENABLE_MASK}, /* 6, SLOT 2, If the frontend has GPIO interrupts, this needs to be disabled. */
    {"EXT_IRQ_3_CPU_INTR",       INT_ENABLE_MASK}, /* 7 */
    {"EXT_IRQ_4_CPU_INTR",       INT_ENABLE_MASK}, /* 8, SLOT 0 */
    {"PCI_SATA_PCI_INTR",        0}, /* 9 */
    {"EXT_IRQ_5_CPU_INTR",       INT_ENABLE_MASK}, /* 10 */
    {"EXT_IRQ_6_CPU_INTR",       0}, /* 11 */
    {"EXT_IRQ_7_CPU_INTR",       INT_ENABLE_MASK}, /* 12, SLOT 2 */
    {"EXT_IRQ_8_CPU_INTR",       0}, /* 13 */
    {"EXT_IRQ_9_CPU_INTR",       0}, /* 14 */
    {"EXT_IRQ_10_CPU_INTR",      INT_ENABLE_MASK}, /* 15 */
    {"EXT_IRQ_11_CPU_INTR",      INT_ENABLE_MASK}, /* 16 */
    {"EXT_IRQ_12_CPU_INTR",      INT_ENABLE_MASK}, /* 17 */
    {"EXT_IRQ_13_CPU_INTR",      INT_ENABLE_MASK}, /* 18, 3517 */
    {"EXT_IRQ_14_CPU_INTR",      INT_ENABLE_MASK}, /* 19, 4500 */
    {"IPI0_CPI_INTR",            INT_ENABLE_MASK}, /* 20 */
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 21 */
    {NULL,                       0}, /* 22 */
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
    {"NMI_PIN_CPU_INTR",         INT_ENABLE_MASK}, /* NMI_PIN_CPU_INTR */
    {NULL,                      0}, /* 4 */
    {NULL,                      0}, /* 5 */
    {NULL,                      0}, /* 6 */
    {NULL,                      0}, /* 7 */
    {NULL,                      0}, /* 8 */
    {NULL,                      0}, /* 9 */
    {NULL,                      0}, /* 10 */
    {NULL,                      0}, /* 11 */
    {NULL,                      0}, /* 12 */
    {NULL,                      0}, /* 13 */
    {NULL,                      0}, /* 14 */
    {NULL,                      0}, /* 15 */
    {NULL,                      0}, /* 16 */
    {NULL,                      0}, /* 17 */
    {NULL,                      0}, /* 18 */
    {NULL,                      0}, /* 19 */
    {NULL,                      0}, /* 20 */
    {NULL,                      0}, /* 21 */
    {NULL,                      0}, /* 22 */
    {NULL,                      0}, /* 23 */
    {NULL,                      0}, /* 24 */
    {NULL,                      0}, /* 25 */
    {NULL,                      0}, /* 26 */
    {NULL,                      0}, /* 27 */
    {NULL,                      0}, /* 28 */
    {NULL,                      0}, /* 29 */
    {NULL,                      0}, /* 30 */
    {NULL,                      0}  /* 31 */
};


s_ChipConfig  g_sChipConfig ={ "7405",interruptName7405, 0xb0001400, sizeof(interruptName7405)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
