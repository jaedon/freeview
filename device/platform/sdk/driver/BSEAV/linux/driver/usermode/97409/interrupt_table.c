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
 * $brcm_Revision: 3 $
 * $brcm_Date: 12/4/11 3:04p $
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
 * 
 * Revision History:
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


s_InteruptTable interruptName7409[] = {
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
    {"RFM1_CPU_INTR",            INT_ENABLE_MASK},/* 16 */
    {"UPG_TMR_CPU_INTR",         INT_ENABLE_MASK},/* 17 */
    {"UPG_CPU_INTR",             INT_ENABLE_MASK}, /* 18 */
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 19 */
    {"UPG_SPI_CPU_INTR",         INT_ENABLE_MASK}, /* 20 */
    {"UPG_UART0_CPU_INTR",       0}, /* 21 */
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 22 */
    {"SUN_CPU_INTR",             INT_ENABLE_MASK}, /* 23 */
    {"UHF1_CPU_INTR",            INT_ENABLE_MASK}, /* 24 */
    {"FW1394_CPU_INTR",          0}, /* 25 */
    {"USB_CPU_INTR",             0}, /* 26 */
    {"MC_CPU_INTR",              INT_SHARABLE | INT_ENABLE_MASK}, /* 27 */
    {"BVNF_CPU_INTR_3",          INT_ENABLE_MASK}, /* 28 */
    {"SATA_PCIB_CPU_INTR",       0}, /* 29 */
    {"AVD0_CPU_INTR",            INT_ENABLE_MASK}, /* 30 */
    {"XPT_RAV_CPU_INTR",         INT_ENABLE_MASK}, /* 31 */
    {"PCI_INTA_0_CPU_INTR",      INT_ENABLE_MASK | INT_REG_WRAP}, /* 0 */
    {"PCI_INTA_1_CPU_INTR",      0}, /* 1 */
    {"PCI_INTA_2_CPU_INTR",      0}, /* 2 */
    {"RFM2_CPU_INTR",            INT_ENABLE_MASK}, /* 3 */
    {"EXT_IRQ_0_CPU_INTR",       INT_ENABLE_MASK}, /* 4 */
    {"EXT_IRQ_1_CPU_INTR",       0}, /* 5 */
    {"EXT_IRQ_2_CPU_INTR",       INT_ENABLE_MASK}, /* 6, SLOT 2 */
    {"EXT_IRQ_3_CPU_INTR",       INT_ENABLE_MASK}, /* 7 */
    {"EXT_IRQ_4_CPU_INTR",       INT_ENABLE_MASK}, /* 8, SLOT 0 */
    {"PCI_SATA_CPU_INTR",        0}, /* 9 */
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
    {"IPI0_CPU_INTR",            INT_ENABLE_MASK}, /* 20 */
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 21 */
    {"AVD1_CPU_INTR",            INT_ENABLE_MASK}, /* 22 */
    {"USB_EHCI_1_CPU_INTR",      0}, /* 23 */
    {"SM_CPU_INTR",              0}, /* 24 */
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 25 */
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 26 */
    {"XPT_MSG_STAT_CPU_INTR",    INT_ENABLE_MASK}, /* 27 */
    {"USB_EHCI_0_CPU_INTR",      0}, /* 28 */
    {"USB_OHCI_0_CPU_INTR",      0}, /* 29 */
    {"USB_OHCI_1_CPU_INTR",      0}, /* 30 */
    {"UHF2_CPU_INTR",            INT_ENABLE_MASK},  /* 31 */
    {"UPG_UART1_CPU_INTR",       0 | INT_REG_WRAP}, /* 0 */
    {"UPG_UART2_CPU_INTR",       0}, /* 1 */
    {"NMI_INTR_CPU_INTR",        INT_ENABLE_MASK}, /* 2 */
    {"MOCA_INTR_CPU_INTR",       0}, /* 3 */
    {"MOCA_ENET_INTR_CPU_INTR",  0}, /* 4 */
    {"SUNDRY_PM_INTR_CPU_INTR",  0}, /* 5 */
    {"MEMC_1_CPU_INTR",          INT_ENABLE_MASK}, /* 6 */
    {"PCIE_NMI__CPU_INTR",       0}, /* 7 */
    {"PCIE_INTA__CPU_INTR",      0}, /* 8 */
    {"PCIE_INTB__CPU_INTR",      0}, /* 9 */
    {"PCIE_INTC__CPU_INTR",      0}, /* 10 */
    {"PCIE_INTD__CPU_INTR",      0}, /* 11 */
    {"PCIE_INTR__CPU_INTR",      0}, /* 12 */
    {"PCIE_ERR_ATTN_CPU_INTR",   0}, /* 13 */
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


s_ChipConfig  g_sChipConfig ={ "7409",interruptName7409, 0xb0441400, sizeof(interruptName7409)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
