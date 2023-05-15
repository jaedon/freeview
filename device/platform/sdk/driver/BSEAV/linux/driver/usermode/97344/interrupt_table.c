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
 * $brcm_Revision: 6 $
 * $brcm_Date: 12/2/11 3:48p $
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/97344/interrupt_table.c $
 * 
 * 6   12/2/11 3:48p gmohile
 * SW7231-506 : Do not enable SYS_PM interrupt
 * 
 * 5   10/3/11 6:36p vishk
 * SW3128-52:  Once tuned to a certain frequency, can't tune to any other
 * frequencies on 3123.
 * 
 * 4   4/28/11 5:00p randyjew
 * SW7344-9: Add 2637 support
 * 
 * 3   12/10/10 11:05a jrubio
 * SW7344-9: put correct start of HIF address
 * 
 * 2   12/8/10 9:25a jrubio
 * SW7344-9: fix table name
 * 
 * 1   12/2/10 4:18p jrubio
 * SW7344-9: initial version of interrupt file
 * 
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


s_InteruptTable interruptName7344[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"AIO_CPU_INTR",             INT_ENABLE_MASK},/* 0 */
    {"BSP_CPU_INTR",        	 INT_ENABLE_MASK},/* 1 */
    {"BVNB_0_CPU_INTR",        	 INT_ENABLE_MASK},/* 2 */
    {"BVNF_0_CPU_INTR",          INT_ENABLE_MASK},/* 3 */
    {"BVNF_1_CPU_INTR",       	 INT_ENABLE_MASK},/* 4 */
    {"BVNF_5_CPU_INTR",          INT_ENABLE_MASK},/* 5 */
    {"BVNM_0_CPU_INTR",          INT_ENABLE_MASK},/* 6 */
    {"CLKGEN_CPU_INTR",          INT_ENABLE_MASK},/* 7 */
    {"EXT_IRQ_00_CPU_INTR",      INT_ENABLE_MASK},/* 8 */
    {"EXT_IRQ_01_CPU_INTR",      INT_ENABLE_MASK},/* 9 */
    {"EXT_IRQ_02_CPU_INTR",      INT_ENABLE_MASK},/* 10 */
    {"EXT_IRQ_03_CPU_INTR",      INT_ENABLE_MASK},/* 11 */
    {"EXT_IRQ_04_CPU_INTR",      INT_ENABLE_MASK},/* 12 */
    {"EXT_IRQ_05_CPU_INTR",      0}, /* 13 */
    {"EXT_IRQ_06_CPU_INTR",      0}, /* 14 */
    {"EXT_IRQ_07_CPU_INTR",      0}, /* 15 */
    {"EXT_IRQ_08_CPU_INTR",      0}, /* 16 */
    {"EXT_IRQ_09_CPU_INTR",      0}, /* 17 */
    {"EXT_IRQ_10_CPU_INTR",      INT_ENABLE_MASK}, /* 18 */
    {"EXT_IRQ_11_CPU_INTR",      0}, /* 19 */
    {"EXT_IRQ_12_CPU_INTR",      0}, /* 20 */
    {"EXT_IRQ_13_CPU_INTR",      0}, /* 21 */
    {"EXT_IRQ_14_CPU_INTR",      0}, /* 22 */
    {"EXT_IRQ_15_CPU_INTR",      0}, /* 23 */
    {"EXT_IRQ_16_CPU_INTR",      0}, /* 24 */
    {"EXT_IRQ_17_CPU_INTR",      0}, /* 25 */
    {"EXT_IRQ_18_CPU_INTR",      0}, /* 26 */
    {"EXT_IRQ_19_CPU_INTR",      0}, /* 27 */
    {"EXT_IRQ_20_CPU_INTR",      0}, /* 28 */
    {"FTM_CPU_INTR",          INT_ENABLE_MASK}, /* 29 */
    {"GENET_0_A_CPU_INTR",       0}, /* 30 */
    {"GENET_0_B_CPU_INTR",       0}, /* 31 */
    {"GENET_1_A_CPU_INTR",       0| INT_REG_WRAP}, /* 0 */
    {"GENET_1_B_CPU_INTR",       0}, /* 1 */
    {"GFX_CPU_INTR",       		 INT_ENABLE_MASK}, /* 2 */
    {"HDMI_TX_CPU_INTR",         INT_ENABLE_MASK}, /* 3 */
    {"HIF_CPU_INTR",             0}, /* 4 */
    {"HIF_SPI_CPU_INTR",         0}, /* 5 */
    {"IPI0_CPU_INTR",            INT_ENABLE_MASK}, /* 6 */
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 7 */
    {"MEMC0_CPU_INTR",           INT_ENABLE_MASK}, /* 8 */
    {"MOCA_CPU_INTR",            0}, /* 9 */
    {"NMI_PIN_CPU_INTR",         INT_ENABLE_MASK}, /* 10 */
    {"RAAGA_CPU_INTR",       	 INT_ENABLE_MASK}, /* 11 */
    {"RAAGA_FW_CPU_INTR",        INT_ENABLE_MASK}, /* 12 */
    {"SDS0_AFEC_0_CPU_INTR",         INT_ENABLE_MASK }, /* 13 */
    {"SDS0_RCVR_0_CPU_INTR",          INT_ENABLE_MASK}, /* 14 */
    {"SDS0_RCVR_1_CPU_INTR",          INT_ENABLE_MASK}, /* 15 */
    {"SDS0_TFEC_0_CPU_INTR",          INT_ENABLE_MASK}, /* 16 */
    {"SOFT_MODEM_CPU_INTR",      INT_ENABLE_MASK}, /* 17 */
    {"SVD0_0_CPU_INTR",          INT_ENABLE_MASK}, /* 18 */
    {"SYS_CPU_INTR",       		 INT_ENABLE_MASK}, /* 19 */
    {"SYS_AON_CPU_INTR",         INT_ENABLE_MASK}, /* 20 */
    {"SYS_PM_CPU_INTR",       	 0}, /* 21 */
    {"UHF_CPU_INTR",      		 INT_ENABLE_MASK}, /* 22 */
    {"UPG_AUX_CPU_INTR",      	 INT_ENABLE_MASK}, /* 23 */
    {"UPG_AUX_AON_CPU_INTR",     INT_ENABLE_MASK}, /* 24 */
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 25 */
    {"UPG_BSC_AON_CPU_INTR",     INT_ENABLE_MASK}, /* 26 */
    {"UPG_MAIN_CPU_INTR",      	 INT_ENABLE_MASK}, /* 27 */
    {"UPG_MAIN_AON_CPU_INTR",    INT_ENABLE_MASK}, /* 28 */
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 29 */
    {"UPG_SPI_CPU_INTR",       	 INT_ENABLE_MASK}, /* 30 */
    {"UPG_TMR_CPU_INTR",         INT_ENABLE_MASK}, /* 31 */
    {"UPG_UART0_CPU_INTR",       0 | INT_REG_WRAP }, /* 0 */
    {"UPG_UART1_CPU_INTR",       0}, /* 1 */
    {"UPG_UART2_CPU_INTR",       0},  /* 2 */
    {"USB0_BRIDGE_CPU_INTR",     0}, /* 3 */
    {"USB0_EHCI_0_CPU_INTR",     0}, /* 4 */
    {"USB0_EHCI_1_CPU_INTR",     0}, /* 5 */
    {"USB0_OHCI_0_CPU_INTR",     0}, /* 6 */
    {"USB0_OHCI_1_CPU_INTR",     0}, /* 7 */
    {"USB1_BRIDGE_CPU_INTR",     0}, /* 8 */
    {"USB1_EHCI_0_CPU_INTR",     0}, /* 9 */
    {"USB1_EHCI_1_CPU_INTR ",    0}, /* 10 */
    {"USB1_OHCI_0_CPU_INTR",   	 0}, /* 11 */
    {"USB1_OHCI_1_CPU_INTR",   	 0}, /* 12 */
    {"VEC_CPU_INTR",      	     INT_ENABLE_MASK}, /* 13 */
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 14 */
    {"XPT_MSG_CPU_INTR",   		 INT_ENABLE_MASK}, /* 15 */
    {"XPT_MSG_STAT_CPU_INTR",    INT_ENABLE_MASK}, /* 16 */
    {"XPT_OVFL_CPU_INTR",        INT_ENABLE_MASK}, /* 17 */
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 18 */
    {"XPT_RAV_CPU_INTR",         INT_ENABLE_MASK}, /* 19 */
    {"XPT_STATUS_CPU_INTR",      INT_ENABLE_MASK}, /* 20 */
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


s_ChipConfig  g_sChipConfig ={ "7344",interruptName7344, 0xb0001400, sizeof(interruptName7344)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
