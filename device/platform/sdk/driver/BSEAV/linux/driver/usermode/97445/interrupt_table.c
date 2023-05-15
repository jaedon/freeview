/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: interrupt_table.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/5/12 6:47p $
 *
 * Module Description:
 * This file contains the platform specific interrupt tables 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/97445/interrupt_table.c $
 * 
 * 1   9/5/12 6:47p katrep
 * SW7429-1:add 7445 interrupt support
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
    

#define NUM_INTC_WORDS   4   /* Number of interrupt controller words */

#ifdef NEXUS_WEBCPU
s_InteruptTable interruptName7445[] = {
    {NULL, 0},                  	 /* Linux IRQ #'s are offset by 1 */
    {"BSP_CPU_INTR",             	0},/* 0 (1)*/
    {"SCPU_CPU_INTR",          		0},/* 1 (2)*/
    {"CPU_ACCESS_VIOL0_CPU_INTR",   0},/* 2 (3)*/
    {"CPU_ACCESS_VIOL1_CPU_INTR",   0},/* 3 (4)*/
    {"NMI_PIN_CPU_INTR",          	0},/* 4 (5)*/
    {"AIO_CPU_INTR",          		0},/* 5 (6)*/
    {"BVNB_0_CPU_INTR",          	0},/* 6 (7)*/
    {"BVNB_1_CPU_INTR",          	0},/* 7 (8)*/
    {"BVNF_0_CPU_INTR",          	0},/* 8 (9)*/
    {"BVNF_1_CPU_INTR",          	0},/* 9 (10)*/
    {"BVNF_5_CPU_INTR",          	0},/* 10 (11)*/
    {"BVNF_8_CPU_INTR",      		0},/* 11 (12)*/
    {"BVNF_9_CPU_INTR",      		0},/* 12 (13)*/
    {"BVNM_0_CPU_INTR",      		0},/* 13 (14)*/
    {"BVNM_1_CPU_INTR",      		0},/* 14 (15)*/
    {"CLKGEN_CPU_INTR",      		0},/* 15 (16)*/
    {"AVS_CPU_INTR",      			0}, /* 16 (17)*/
    {"DVP_HR_CPU_INTR",       		0}, /* 17 (18)*/
    {"HDMI_TX_CPU_INTR",       		0}, /* 18 (19)*/
    {"HDMI_RX_0_CPU_INTR",       	0}, /* 19 (20)*/
    {"GENET_0_A_CPU_INTR",       	0}, /* 20 (21)*/
    {"GENET_0_B_CPU_INTR",          0}, /* 21 (22)*/
    {"GENET_1_A_CPU_INTR",       	0}, /* 22 (23)*/
    {"GENET_1_B_CPU_INTR",         	0}, /* 23 (24)*/
    {"GENET_2_A_CPU_INTR",          0}, /* 24 (25)*/
    {"GENET_2_B_CPU_INTR",         	0}, /* 25 (26)*/
    {"GFX_CPU_INTR",            	0}, /* 26 (27)*/
    {"MEMC0_CPU_INTR",            	0}, /* 27 (28)*/
    {"MEMC1_CPU_INTR",            	0}, /* 28 (29)*/
    {"MOCA_CPU_INTR",            	0}, /* 29 (30)*/
    {"SATA_AHCI_CPU_INTR",          0}, /* 30 (31)*/
    {"SATA_GRB_CPU_INTR",           0}, /* 31 (32)*/

    {"HIF_CPU_INTR",            	0 | INT_REG_WRAP},/* 0 (33)*/
    {"HIF_SPI_CPU_INTR",         	0}, /* 1 (34)*/
    {"IPI0_CPU_INTR",			  	0}, /* 2 (35)*/
    {"IPI1_CPU_INTR",       		0}, /* 3 (36)*/
    {" IPI2_CPU_INTR",       		0}, /* 4 (37)*/
    {" IPI3_CPU_INTR",       		0}, /* 5 (38)*/
    {"SDIO0_0_CPU_INTR",       		0}, /* 6 (39)*/
    {"SDIO1_0_CPU_INTR",      		0}, /* 7 (40)*/
    {"WEBHIF_CPU_INTR",        		0}, /* 8 (41)*/
    {"WEBHIF_IPI0_CPU_INTR",        0}, /* 9 (42)*/
    {"WEBHIF_IPI1_CPU_INTR",        0}, /* 10 (43)*/
    {"WEBHIF_STB_IPI0_CPU_INTR",    0}, /* 11 (44)*/
    {"WEBHIF_STB_IPI1_CPU_INTR",    0}, /* 12 (45)*/
    {"WEBHIF_TIMER_CPU_INTR",       0}, /* 13 (46)*/
    {"PCIE_ERR_ATTN_CPU_INTR",      0}, /* 14 (47)*/
    {"PCIE_INTA_CPU_INTR",         	0}, /* 15 (48)*/
    {"PCIE_INTB_CPU_INTR",         	0}, /* 16 (49)*/
    {"PCIE_INTC_CPU_INTR",          0}, /* 17 (50)*/
    {"PCIE_INTD_CPU_INTR",     		0}, /* 18 (51)*/
    {"PCIE_INTR_CPU_INTR",          0}, /* 19 (52)*/
    {"PCIE_NMI_CPU_INTR",           0}, /* 20 (53)*/
    {"RAAGA_CPU_INTR",         		0}, /* 21 (54)*/
    {"RAAGA_FW_CPU_INTR",          	0}, /* 22 (55)*/
    {"RAAGA1_CPU_INTR",             0}, /* 23 (56)*/
    {"RAAGA1_FW_CPU_INTR",         	0}, /* 24 (57)*/
    {"SHVD0_0_CPU_INTR",     		0}, /* 25 (58)*/
    {"SID0_0_CPU_INTR",         	0}, /* 26 (59)*/
    {"HVD1_0_CPU_INTR",     		0}, /* 27 (60)*/
    {"HVD2_0_CPU_INTR",        		0}, /* 28 (61)*/
    {"SOFT_MODEM_CPU_INTR",    		0}, /* 29 (62)*/
    {"SYS_CPU_INTR",          		0}, /* 30 (63)*/
    {"SYS_AON_CPU_INTR",          	0}, /* 31 (64)*/

    {"SYS_PM_CPU_INTR",         	0 | INT_REG_WRAP}, /* 0 (65)*/
    {"UPG_AUX_CPU_INTR",     	    0}, /* 1 (66)*/
    {"UPG_AUX_AON_CPU_INTR",   	    0}, /* 2 (67)*/
    {"UPG_BSC_CPU_INTR",     		0}, /* 3 (68)*/
    {"UPG_BSC_AON_CPU_INTR",	    0}, /* 4 (69)*/
    {"UPG_MAIN_CPU_INTR",    		0}, /* 5 (70)*/
    {"UPG_MAIN_AON_CPU_INTR",    	0}, /* 6 (71)*/
    {"UPG_MC_CPU_INTR",     		0}, /* 7 (72)*/
    {"UPG_SC_CPU_INTR",     		0}, /* 8 (73)*/
    {"UPG_SPI_CPU_INTR",     		0}, /* 9 (74)*/
    {"UPG_TMR_CPU_INTR",     		0}, /* 10 (75)*/
    {"UPG_UART0_CPU_INTR",    		0}, /* 11 (76)*/
    {"UPG_UART1_CPU_INTR ",    		0}, /* 12 (77)*/
    {"UPG_UART2_CPU_INTR",     		0}, /* 13 (78)*/
    {"USB0_BRIDGE_CPU_INTR",     	0}, /* 14 (79)*/
    {"USB0_EHCI_0_CPU_INTR",        0}, /* 15 (80)*/
    {"USB0_EHCI_1_CPU_INTR",        0}, /* 16 (81)*/
    {"USB0_OHCI_0_CPU_INTR",        0}, /* 17 (82)*/
    {"USB0_OHCI_1_CPU_INTR",        0}, /* 18 (83)*/
    {"USB0_XHCI_0_CPU_INTR",        0}, /* 19 (84)*/
    {"USB0_XHCI_1_CPU_INTR",     	0}, /* 20 (85)*/
    {"USB1_BRIDGE_CPU_INTR",     	0}, /* 21 (86)*/
    {"USB1_EHCI_0_CPU_INTR", 		0}, /* 22 (87)*/
    {"USB1_EHCI_1_CPU_INTR", 		0}, /* 23 (88)*/
    {"USB1_OHCI_0_CPU_INTR",    	0}, /* 24 (89)*/
    {"USB1_OHCI_1_CPU_INTR",        0}, /* 25 (90)*/
    {"V3D_CPU_INTR",         		0}, /* 26 (91)*/
    {"VEC_CPU_INTR",    			0}, /* 27 (92)*/
    {"VICE2_0_CPU_INTR",        	0}, /* 28 (93)*/
    {"VICE2_1_CPU_INTR",         	0}, /* 29 (94)*/
    {"XPT_FE_CPU_INTR",         	0}, /* 30 (95)*/
    {"XPT_MSG_CPU_INTR",      		0}, /* 31 (96)*/

    {"XPT_PCR_CPU_INTR",			0 | INT_REG_WRAP}, /* 0 (97)*/
    {"XPT_RAV_CPU_INTR",			0}, /* 1 (98)*/
    {"XPT_STATUS_CPU_INTR",         0}, /* 2 (99)*/
    {"XPT_MCPB_CPU_INTR",           0}, /* 3 (100)*/
    {"XPT_MDMA_MCPB_CPU_INTR",      0}, /* 4 (101)*/
    {"XPT_WMDMA_CPU_INTR",          0}, /* 5 (102)*/
    {"XPT_EXTCARD_CPU_INTR",        0}, /* 6 (103)*/
    {"M2MC1_CPU_INTR",              INT_ENABLE_MASK}, /* 7 (104)*/
    {"RFM_CPU_INTR",                0}, /* 8 (105)*/
    {"EXT_IRQ_00_CPU_INTR",         0}, /* 9 (106)*/
    {"EXT_IRQ_01_CPU_INTR",         0}, /* 10 (107)*/
    {"EXT_IRQ_02_CPU_INTR",         0}, /* 11 (108)*/
    {"EXT_IRQ_03_CPU_INTR",         0}, /* 12 (109)*/
    {"EXT_IRQ_04_CPU_INTR",         0}, /* 13 (110)*/
    {"EXT_IRQ_05_CPU_INTR",         0}, /* 14 (111)*/
    {"MEMC2_CPU_INTR",              INT_ENABLE_MASK}, /* 15 (112)*/
    {NULL,                     		0}, /* 16 (113)*/
    {NULL,                       	0}, /* 17 (114)*/
    {NULL,                       	0}, /* 18 (115)*/
    {NULL,                       	0}, /* 19 (116)*/
    {NULL,                       	0}, /* 20 (117)*/
    {NULL,                       	0}, /* 21 (118)*/
    {NULL,                       	0}, /* 22 (119)*/
    {NULL,                       	0}, /* 23 (120)*/
    {NULL,                       	0}, /* 24 (121)*/
    {NULL,                       	0}, /* 25 (122)*/
    {NULL,                       	0}, /* 26 (123)*/
    {NULL,                       	0}, /* 27 (124)*/
    {NULL,                       	0}, /* 28 (125)*/
    {NULL,                       	0}, /* 29 (126)*/
    {NULL,                       	0}, /* 30 (127)*/
    {NULL,                       	0}, /* 31 (128)*/
};
#else
/* NEXUS_WEBCPU client */
s_InteruptTable interruptName7445[] = {
        {NULL, 0},                  	 /* Linux IRQ #'s are offset by 1 */
    {"BSP_CPU_INTR",             	0},/* 0 (1)*/
    {"SCPU_CPU_INTR",          		0},/* 1 (2)*/
    {"CPU_ACCESS_VIOL0_CPU_INTR",   INT_ENABLE_MASK},/* 2 (3)*/
    {"CPU_ACCESS_VIOL1_CPU_INTR",   INT_ENABLE_MASK},/* 3 (4)*/
    {"NMI_PIN_CPU_INTR",          	INT_ENABLE_MASK},/* 4 (5)*/
    {"AIO_CPU_INTR",          		INT_ENABLE_MASK},/* 5 (6)*/
    {"BVNB_0_CPU_INTR",          	INT_ENABLE_MASK},/* 6 (7)*/
    {"BVNB_1_CPU_INTR",          	INT_ENABLE_MASK},/* 7 (8)*/
    {"BVNF_0_CPU_INTR",          	INT_ENABLE_MASK},/* 8 (9)*/
    {"BVNF_1_CPU_INTR",          	INT_ENABLE_MASK},/* 9 (10)*/
    {"BVNF_5_CPU_INTR",          	INT_ENABLE_MASK},/* 10 (11)*/
    {"BVNF_8_CPU_INTR",      		INT_ENABLE_MASK},/* 11 (12)*/
    {"BVNF_9_CPU_INTR",      		INT_ENABLE_MASK},/* 12 (13)*/
    {"BVNM_0_CPU_INTR",      		INT_ENABLE_MASK},/* 13 (14)*/
    {"BVNM_1_CPU_INTR",      		INT_ENABLE_MASK},/* 14 (15)*/
    {"CLKGEN_CPU_INTR",      		INT_ENABLE_MASK},/* 15 (16)*/
    {"AVS_CPU_INTR",      			INT_ENABLE_MASK}, /* 16 (17)*/
    {"DVP_HR_CPU_INTR",       		INT_ENABLE_MASK}, /* 17 (18)*/
    {"HDMI_TX_CPU_INTR",       		INT_ENABLE_MASK}, /* 18 (19)*/
    {"HDMI_RX_0_CPU_INTR",       	INT_ENABLE_MASK}, /* 19 (20)*/
    {"GENET_0_A_CPU_INTR",       	0}, /* 20 (21)*/
    {"GENET_0_B_CPU_INTR",          0}, /* 21 (22)*/
    {"GENET_1_A_CPU_INTR",       	0}, /* 22 (23)*/
    {"GENET_1_B_CPU_INTR",         	0}, /* 23 (24)*/
    {"GENET_2_A_CPU_INTR",          0}, /* 24 (25)*/
    {"GENET_2_B_CPU_INTR",         	0}, /* 25 (26)*/
    {"GFX_CPU_INTR",            	INT_ENABLE_MASK}, /* 26 (27)*/
    {"MEMC0_CPU_INTR",            	0}, /* 27 (28)*/
    {"MEMC1_CPU_INTR",            	0}, /* 28 (29)*/
    {"MOCA_CPU_INTR",            	0}, /* 29 (30)*/
    {"SATA_AHCI_CPU_INTR",          0}, /* 30 (31)*/
    {"SATA_GRB_CPU_INTR",           0}, /* 31 (32)*/

    {"HIF_CPU_INTR",            	INT_ENABLE_MASK | INT_REG_WRAP},/* 0 (33)*/
    {"HIF_SPI_CPU_INTR",         	INT_ENABLE_MASK}, /* 1 (34)*/
    {"IPI0_CPU_INTR",			  	0}, /* 2 (35)*/
    {"IPI1_CPU_INTR",       		0}, /* 3 (36)*/
    {" IPI2_CPU_INTR",       		0}, /* 4 (37)*/
    {" IPI3_CPU_INTR",       		0}, /* 5 (38)*/
    {"SDIO0_0_CPU_INTR",       		0}, /* 6 (39)*/
    {"SDIO1_0_CPU_INTR",      		0}, /* 7 (40)*/
    {"WEBHIF_CPU_INTR",        		0}, /* 8 (41)*/
    {"WEBHIF_IPI0_CPU_INTR",        0}, /* 9 (42)*/
    {"WEBHIF_IPI1_CPU_INTR",        0}, /* 10 (43)*/
    {"WEBHIF_STB_IPI0_CPU_INTR",    0}, /* 11 (44)*/
    {"WEBHIF_STB_IPI1_CPU_INTR",    0}, /* 12 (45)*/
    {"WEBHIF_TIMER_CPU_INTR",       0}, /* 13 (46)*/
    {"PCIE_ERR_ATTN_CPU_INTR",      0}, /* 14 (47)*/
    {"PCIE_INTA_CPU_INTR",         	0}, /* 15 (48)*/
    {"PCIE_INTB_CPU_INTR",         	0}, /* 16 (49)*/
    {"PCIE_INTC_CPU_INTR",          0}, /* 17 (50)*/
    {"PCIE_INTD_CPU_INTR",     		0}, /* 18 (51)*/
    {"PCIE_INTR_CPU_INTR",          0}, /* 19 (52)*/
    {"PCIE_NMI_CPU_INTR",           0}, /* 20 (53)*/
    {"RAAGA_CPU_INTR",         		INT_ENABLE_MASK}, /* 21 (54)*/
    {"RAAGA_FW_CPU_INTR",          	INT_ENABLE_MASK}, /* 22 (55)*/
    {"RAAGA1_CPU_INTR",             INT_ENABLE_MASK}, /* 23 (56)*/
    {"RAAGA1_FW_CPU_INTR",         	INT_ENABLE_MASK}, /* 24 (57)*/
    {"SHVD0_0_CPU_INTR",     		INT_ENABLE_MASK}, /* 25 (58)*/
    {"SID0_0_CPU_INTR",         	INT_ENABLE_MASK}, /* 26 (59)*/
    {"HVD1_0_CPU_INTR",     		INT_ENABLE_MASK}, /* 27 (60)*/
    {"HVD2_0_CPU_INTR",        		INT_ENABLE_MASK}, /* 28 (61)*/
    {"SOFT_MODEM_CPU_INTR",    		0}, /* 29 (62)*/
    {"SYS_CPU_INTR",          		INT_ENABLE_MASK}, /* 30 (63)*/
    {"SYS_AON_CPU_INTR",          	INT_ENABLE_MASK}, /* 31 (64)*/

    {"SYS_PM_CPU_INTR",         	0 | INT_REG_WRAP}, /* 0 (65)*/
    {"UPG_AUX_CPU_INTR",     	    INT_ENABLE_MASK}, /* 1 (66)*/
    {"UPG_AUX_AON_CPU_INTR",   	    0}, /* 2 (67)*/
    {"UPG_BSC_CPU_INTR",     		0}, /* 3 (68)*/
    {"UPG_BSC_AON_CPU_INTR",	    0}, /* 4 (69)*/
    {"UPG_MAIN_CPU_INTR",    		0}, /* 5 (70)*/
    {"UPG_MAIN_AON_CPU_INTR",    	0}, /* 6 (71)*/
    {"UPG_MC_CPU_INTR",     		INT_ENABLE_MASK}, /* 7 (72)*/
    {"UPG_SC_CPU_INTR",     		INT_ENABLE_MASK}, /* 8 (73)*/
    {"UPG_SPI_CPU_INTR",     		INT_ENABLE_MASK}, /* 9 (74)*/
    {"UPG_TMR_CPU_INTR",     		INT_ENABLE_MASK}, /* 10 (75)*/
    {"UPG_UART0_CPU_INTR",    		0}, /* 11 (76)*/
    {"UPG_UART1_CPU_INTR ",    		0}, /* 12 (77)*/
    {"UPG_UART2_CPU_INTR",     		0}, /* 13 (78)*/
    {"USB0_BRIDGE_CPU_INTR",     	0}, /* 14 (79)*/
    {"USB0_EHCI_0_CPU_INTR",        0}, /* 15 (80)*/
    {"USB0_EHCI_1_CPU_INTR",        0}, /* 16 (81)*/
    {"USB0_OHCI_0_CPU_INTR",        0}, /* 17 (82)*/
    {"USB0_OHCI_1_CPU_INTR",        0}, /* 18 (83)*/
    {"USB0_XHCI_0_CPU_INTR",        0}, /* 19 (84)*/
    {"USB0_XHCI_1_CPU_INTR",     	0}, /* 20 (85)*/
    {"USB1_BRIDGE_CPU_INTR",     	0}, /* 21 (86)*/
    {"USB1_EHCI_0_CPU_INTR", 		0}, /* 22 (87)*/
    {"USB1_EHCI_1_CPU_INTR", 		0}, /* 23 (88)*/
    {"USB1_OHCI_0_CPU_INTR",    	0}, /* 24 (89)*/
    {"USB1_OHCI_1_CPU_INTR",        0}, /* 25 (90)*/
    {"V3D_CPU_INTR",         		INT_ENABLE_MASK}, /* 26 (91)*/
    {"VEC_CPU_INTR",    			INT_ENABLE_MASK}, /* 27 (92)*/
    {"VICE2_0_CPU_INTR",        	INT_ENABLE_MASK}, /* 28 (93)*/
    {"VICE2_1_CPU_INTR",         	INT_ENABLE_MASK}, /* 29 (94)*/
    {"XPT_FE_CPU_INTR",         	INT_ENABLE_MASK}, /* 30 (95)*/
    {"XPT_MSG_CPU_INTR",      		INT_ENABLE_MASK}, /* 31 (96)*/

    {"XPT_PCR_CPU_INTR",			INT_ENABLE_MASK | INT_REG_WRAP}, /* 0 (97)*/
    {"XPT_RAV_CPU_INTR",			INT_ENABLE_MASK}, /* 1 (98)*/
    {"XPT_STATUS_CPU_INTR",         INT_ENABLE_MASK}, /* 2 (99)*/
    {"XPT_MCPB_CPU_INTR",           INT_ENABLE_MASK}, /* 3 (100)*/
    {"XPT_MDMA_MCPB_CPU_INTR",      INT_ENABLE_MASK}, /* 4 (101)*/
    {"XPT_WMDMA_CPU_INTR",          INT_ENABLE_MASK}, /* 5 (102)*/
    {"XPT_EXTCARD_CPU_INTR",        INT_ENABLE_MASK}, /* 6 (103)*/
    {"M2MC1_CPU_INTR",              INT_ENABLE_MASK}, /* 7 (104)*/
    {"RFM_CPU_INTR",                INT_ENABLE_MASK}, /* 8 (105)*/
    {"EXT_IRQ_00_CPU_INTR",         INT_ENABLE_MASK}, /* 9 (106)*/
    {"EXT_IRQ_01_CPU_INTR",         INT_ENABLE_MASK}, /* 10 (107)*/
    {"EXT_IRQ_02_CPU_INTR",         INT_ENABLE_MASK}, /* 11 (108)*/
    {"EXT_IRQ_03_CPU_INTR",         INT_ENABLE_MASK}, /* 12 (109)*/
    {"EXT_IRQ_04_CPU_INTR",         INT_ENABLE_MASK}, /* 13 (110)*/
    {"EXT_IRQ_05_CPU_INTR",         INT_ENABLE_MASK}, /* 14 (111)*/
    {"MEMC2_CPU_INTR",              INT_ENABLE_MASK}, /* 15 (112)*/
    {NULL,                       0}, /* 16 (113)*/
    {NULL,                       0}, /* 17 (114)*/
    {NULL,                       0}, /* 18 (115)*/
    {NULL,                       0}, /* 19 (116)*/
    {NULL,                       0}, /* 20 (117)*/
    {NULL,                       0}, /* 21 (118)*/
    {NULL,                       0}, /* 22 (119)*/
    {NULL,                       0}, /* 23 (120)*/
    {NULL,                       0}, /* 24 (121)*/
    {NULL,                       0}, /* 25 (122)*/
    {NULL,                       0}, /* 26 (123)*/
    {NULL,                       0}, /* 27 (124)*/
    {NULL,                       0}, /* 28 (125)*/
    {NULL,                       0}, /* 29 (126)*/
    {NULL,                       0}, /* 30 (127)*/
    {NULL,                       0}, /* 31 (128)*/
};
#endif

s_ChipConfig  g_sChipConfig ={ "7445",interruptName7445, 0x0, sizeof(interruptName7445)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
