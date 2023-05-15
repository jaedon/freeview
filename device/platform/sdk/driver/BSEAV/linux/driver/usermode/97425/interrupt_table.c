/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
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
#if defined(HUMAX_PLATFORM_BASE)
#define REGOFFSET_TO_ADDR(offset) (KSEG1 + 0x10000000 + offset)
#define BCM7425_UPG_MANAGED_BITS 0x00000040

s_InteruptTable interruptName7425L2 = {
	{"GPIO", 0}
};

#define HMS_GPIO_MANAGED_BITS 0x40000000

void bcm7425_upg_l2share_func(int irq, unsigned long *triggeredInts)
{
	volatile unsigned long *l2_base = (volatile unsigned long *)REGOFFSET_TO_ADDR(0x406780);//(volatile unsigned long *)0xB0406780;
	volatile unsigned long *gio_mask = (volatile unsigned long *)REGOFFSET_TO_ADDR(0x406754);
	volatile unsigned long status, enable;
	volatile unsigned long right = 0;
    
	status = l2_base[1];
	enable = l2_base[0];

	right = gio_mask[0] & gio_mask[2] & HMS_GPIO_MANAGED_BITS;
	*triggeredInts = status & enable & BCM7425_UPG_MANAGED_BITS;


	
	if(!right){
		//printk("gio_status = %x\n", gio_mask[2]);
		 l2_base[0] = l2_base[0] & ~(*triggeredInts);
		return;
	}
	else{
		//printk("ls2hare, int=%x\n", *triggeredInts);
		
	}
	
	 if (*triggeredInts) {
        l2_base[0] = l2_base[0] & ~(*triggeredInts);
		//gio_mask[0] &= ~(HMS_GPIO_MANAGED_BITS);
		//gio_mask[1] |= HMS_GPIO_MANAGED_BITS;
		if(*triggeredInts & 0x40)
			interruptName7425L2.numInter++;
    }

}

/* function to mask the managed interrupts */
void bcm7425_upg_l2mask_func(int irq, unsigned long *unused)
{
        
}
#endif


s_InteruptTable interruptName7425[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"AIO_CPU_INTR",             INT_ENABLE_MASK},/* 0 */
    {"AVD1_0_CPU_INTR",      	 INT_ENABLE_MASK},/* 1 */
    {"BSP_CPU_INTR",        	 INT_ENABLE_MASK},/* 2 */
    {"BVNB_0_CPU_INTR",        	 INT_ENABLE_MASK},/* 3 */
    {"BVNF_0_CPU_INTR",          INT_ENABLE_MASK},/* 4 */
    {"BVNF_1_CPU_INTR",       	 INT_ENABLE_MASK},/* 5 */
    {"BVNF_5_CPU_INTR",          INT_ENABLE_MASK},/* 6 */
    {"BVNM_0_CPU_INTR",          INT_ENABLE_MASK},/* 7 */
#if BCHP_VER >= BCHP_VER_B0
    {"BVNM_1_CPU_INTR",          INT_ENABLE_MASK},/* 8 */
#endif
    {"CLKGEN_CPU_INTR",          INT_ENABLE_MASK},/* 8 */
    {"DVP_HR_CPU_INTR",          INT_ENABLE_MASK},/* 9 */
    {"EXT_IRQ_00_CPU_INTR",      INT_ENABLE_MASK},/* 10 */
    {"EXT_IRQ_01_CPU_INTR",      INT_ENABLE_MASK},/* 11 */
    {"EXT_IRQ_02_CPU_INTR",      INT_ENABLE_MASK},/* 12 */
    {"EXT_IRQ_03_CPU_INTR",      INT_ENABLE_MASK},/* 13 */
    {"EXT_IRQ_04_CPU_INTR",      INT_ENABLE_MASK},/* 14 */
    {"EXT_IRQ_05_CPU_INTR",      0}, /* 15 */
#if BCHP_VER < BCHP_VER_B0
    {"FW1394_CPU_INTR",          0}, /* 16 */
#endif
    {"GENET_0_A_CPU_INTR",       0}, /* 17 */
    {"GENET_0_B_CPU_INTR",       0}, /* 18 */
    {"GENET_1_A_CPU_INTR",       0}, /* 19 */
    {"GENET_1_B_CPU_INTR",       0}, /* 20 */
    {"GFX_CPU_INTR",       		 INT_ENABLE_MASK}, /* 21 */
    {"HDMI_RX_0_CPU_INTR",       INT_ENABLE_MASK}, /* 22 */
    {"HDMI_TX_CPU_INTR",         INT_ENABLE_MASK}, /* 23 */
    {"HIF_CPU_INTR",             0}, /* 24 */
    {"HIF_SPI_CPU_INTR",         0}, /* 25 */
    {"IPI0_CPU_INTR",            INT_ENABLE_MASK}, /* 26 */
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 27 */
    {"MEMC0_CPU_INTR",           INT_ENABLE_MASK}, /* 28 */
    {"MEMC1_CPU_INTR",           INT_ENABLE_MASK}, /* 29 */
    {"MOCA_CPU_INTR",            0}, /* 30 */
    {"NMI_PIN_CPU_INTR",         INT_ENABLE_MASK}, /* 31 */
    {"PCIE_ERR_ATTN_CPU_INTR",   0 | INT_REG_WRAP}, /* 0 */
    {"PCIE_INTA_CPU_INTR",       0}, /* 1 */
    {"PCIE_INTB_CPU_INTR",       0}, /* 2 */
    {"PCIE_INTC_CPU_INTR",       0}, /* 3 */
    {"PCIE_INTD_CPU_INTR",       0}, /* 4 */
    {"PCIE_INTR_CPU_INTR",       0}, /* 5 */
    {"PCIE_NMI_CPU_INTR",        0}, /* 6 */
    {"RAAGA_CPU_INTR",       	 INT_ENABLE_MASK}, /* 7 */
    {"RAAGA_FW_CPU_INTR",        INT_ENABLE_MASK}, /* 8 */
    {"SATA_0_CPU_INTR",          0}, /* 9 */
    {"SATA_1_CPU_INTR",          0}, /* 10 */
    {"SDIO0_0_CPU_INTR",         0}, /* 11 */
    {"SDIO1_0_CPU_INTR",         0}, /* 12 */
    {"SOFT_MODEM_CPU_INTR",      INT_ENABLE_MASK}, /* 13 */
    {"SVD0_0_CPU_INTR",          INT_ENABLE_MASK}, /* 14 */
    {"SYS_CPU_INTR",       		 INT_ENABLE_MASK}, /* 15 */
    {"SYS_AON_CPU_INTR",         INT_ENABLE_MASK}, /* 16 */
    {"SYS_PM_CPU_INTR",       	 0}, /* 17 */
    {"UHF_CPU_INTR",      		 INT_ENABLE_MASK}, /* 18 */
    {"UPG_AUX_CPU_INTR",      	 INT_ENABLE_MASK}, /* 19 */
    {"UPG_AUX_AON_CPU_INTR",     INT_ENABLE_MASK}, /* 20 */
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 21 */
    {"UPG_BSC_AON_CPU_INTR",     INT_ENABLE_MASK}, /* 22 */
#if defined(HUMAX_PLATFORM_BASE)
    {"UPG_MAIN_CPU_INTR",      	 INT_ENABLE_MASK,  0, bcm7425_upg_l2share_func, NULL, 0}, /* 23 */
#else
    {"UPG_MAIN_CPU_INTR",      	 INT_ENABLE_MASK}, /* 23 */
#endif
    {"UPG_MAIN_AON_CPU_INTR",    INT_ENABLE_MASK}, /* 24 */
    {"UPG_MC_CPU_INTR",       	 0}, /* 25 */
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 26 */
    {"UPG_SPI_CPU_INTR",       	 INT_ENABLE_MASK}, /* 27 */
    {"UPG_TMR_CPU_INTR",         INT_ENABLE_MASK}, /* 28 */
    {"UPG_UART0_CPU_INTR",       0}, /* 29 */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
    {"UPG_UART1_CPU_INTR",		 INT_ENABLE_MASK}, /* 30 */
    {"UPG_UART2_CPU_INTR",		 INT_ENABLE_MASK}, /* 31 */
#else
    {"UPG_UART1_CPU_INTR",       0}, /* 30 */
    {"UPG_UART2_CPU_INTR",       0},  /* 31 */
#endif
    {"USB0_BRIDGE_CPU_INTR",     0 | INT_REG_WRAP}, /* 0 */
    {"USB0_EHCI_0_CPU_INTR",     0}, /* 1 */
    {"USB0_EHCI_1_CPU_INTR",     0}, /* 2 */
    {"USB0_OHCI_0_CPU_INTR",     0}, /* 3 */
    {"USB0_OHCI_1_CPU_INTR",     0}, /* 4 */
    {"USB1_BRIDGE_CPU_INTR",     0}, /* 5 */
    {"USB1_EHCI_0_CPU_INTR",     0}, /* 6 */
    {"USB1_EHCI_1_CPU_INTR ",    0}, /* 7 */
    {"USB1_OHCI_0_CPU_INTR",   	 0}, /* 8 */
    {"USB1_OHCI_1_CPU_INTR",   	 0}, /* 9 */
#if BCHP_VER >= BCHP_VER_B0
    {"V3D_CPU_INTR",      	     INT_ENABLE_MASK}, /* 10 */
#endif
    {"VEC_CPU_INTR",      	     INT_ENABLE_MASK}, /* 10 */
    {"VICE0_0_CPU_INTR",    	 INT_ENABLE_MASK}, /* 11 */
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 12 */
    {"XPT_MSG_CPU_INTR",   		 INT_ENABLE_MASK}, /* 13 */
    {"XPT_MSG_STAT_CPU_INTR",    INT_ENABLE_MASK}, /* 14 */
    {"XPT_OVFL_CPU_INTR",        INT_ENABLE_MASK}, /* 15 */
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 16 */
    {"XPT_RAV_CPU_INTR",         INT_ENABLE_MASK}, /* 17 */
    {"XPT_STATUS_CPU_INTR",      INT_ENABLE_MASK}, /* 18 */
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
#if BCHP_VER < BCHP_VER_B0 /* keep the 32-bit for mask[2] */
    {NULL,                       0}  /* 31 */
#endif
};


s_ChipConfig  g_sChipConfig ={ "7425",interruptName7425, 0xb041a400, sizeof(interruptName7425)/sizeof(s_InteruptTable),NUM_INTC_WORDS};