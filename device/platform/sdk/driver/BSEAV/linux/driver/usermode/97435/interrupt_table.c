/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
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
    

#define NUM_INTC_WORDS   4   /* Number of interrupt controller words */

#ifndef NEXUS_WEBCPU
s_InteruptTable interruptName7435[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {"AIO_CPU_INTR",             INT_ENABLE_MASK},/* 0 (1)*/
    {"AVD1_0_CPU_INTR",          INT_ENABLE_MASK},/* 1 (2)*/
    {"BSP_CPU_INTR",             INT_ENABLE_MASK},/* 2 (3)*/
    {"BVNB_0_CPU_INTR",          INT_ENABLE_MASK},/* 3 (4)*/
    {"BVNF_0_CPU_INTR",          INT_ENABLE_MASK},/* 4 (5)*/
    {"BVNF_1_CPU_INTR",          INT_ENABLE_MASK},/* 5 (6)*/
    {"BVNF_5_CPU_INTR",          INT_ENABLE_MASK},/* 6 (7)*/
    {"BVNM_0_CPU_INTR",          INT_ENABLE_MASK},/* 7 (8)*/
    {"BVNM_1_CPU_INTR",          INT_ENABLE_MASK},/* 8 (9)*/
    {"CLKGEN_CPU_INTR",          INT_ENABLE_MASK},/* 9 (10)*/
    {"DVP_HR_CPU_INTR",          INT_ENABLE_MASK},/* 10 (11)*/
    {"EXT_IRQ_00_CPU_INTR",      INT_ENABLE_MASK},/* 11 (12)*/
    {"EXT_IRQ_01_CPU_INTR",      INT_ENABLE_MASK},/* 12 (13)*/
    {"EXT_IRQ_02_CPU_INTR",      INT_ENABLE_MASK},/* 13 (14)*/
    {"EXT_IRQ_03_CPU_INTR",      INT_ENABLE_MASK},/* 14 (15)*/
    {"EXT_IRQ_04_CPU_INTR",      INT_ENABLE_MASK},/* 15 (16)*/
    {"EXT_IRQ_05_CPU_INTR",      0}, /* 16 (17)*/
    {"GENET_0_A_CPU_INTR",       0}, /* 17 (18)*/
    {"GENET_0_B_CPU_INTR",       0}, /* 18 (19)*/
    {"GENET_1_A_CPU_INTR",       0}, /* 19 (20)*/
    {"GENET_1_B_CPU_INTR",       0}, /* 20 (21)*/
    {"GFX_CPU_INTR",             INT_ENABLE_MASK}, /* 21 (22)*/
    {"HDMI_RX_0_CPU_INTR",       INT_ENABLE_MASK}, /* 22 (23)*/
    {"HDMI_TX_CPU_INTR",         INT_ENABLE_MASK}, /* 23 (24)*/
    {"HIF_CPU_INTR",             0}, /* 24 (25)*/
    {"HIF_SPI_CPU_INTR",         0}, /* 25 (26)*/
    {"IPI0_CPU_INTR",            INT_ENABLE_MASK}, /* 26 (27)*/
    {"IPI1_CPU_INTR",            INT_ENABLE_MASK}, /* 27 (28)*/
    {"IPI2_CPU_INTR",            INT_ENABLE_MASK}, /* 28 (29)*/
    {"IPI3_CPU_INTR",            INT_ENABLE_MASK}, /* 29 (30)*/
    {"MEMC0_CPU_INTR",           INT_ENABLE_MASK}, /* 30 (31)*/
    {"MEMC1_CPU_INTR",           INT_ENABLE_MASK}, /* 31 (32)*/
    {"MOCA_CPU_INTR",            0 | INT_REG_WRAP},/* 0 (33)*/
    {"NMI_PIN_CPU_INTR",         INT_ENABLE_MASK}, /* 1 (34)*/
    {"PCIE_ERR_ATTN_CPU_INTR",   0}, /* 2 (35)*/
    {"PCIE_INTA_CPU_INTR",       0}, /* 3 (36)*/
    {"PCIE_INTB_CPU_INTR",       0}, /* 4 (37)*/
    {"PCIE_INTC_CPU_INTR",       0}, /* 5 (38)*/
    {"PCIE_INTD_CPU_INTR",       0}, /* 6 (39)*/
    {"PCIE_INTR_CPU_INTR",       0}, /* 7 (40)*/
    {"PCIE_NMI_CPU_INTR",        0}, /* 8 (41)*/
    {"RAAGA_CPU_INTR",           INT_ENABLE_MASK}, /* 9 (42)*/
    {"RAAGA_FW_CPU_INTR",        INT_ENABLE_MASK}, /* 10 (43)*/
    {"RAAGA1_CPU_INTR",          INT_ENABLE_MASK}, /* 11 (44)*/
    {"RAAGA1_FW_CPU_INTR",       INT_ENABLE_MASK}, /* 12 (45)*/
    {"SATA_AHCI_CPU_INTR",       0}, /* 13 (46)*/
    {"SATA_GRB_CPU_INTR",        0}, /* 14 (47)*/
    {"SDIO0_0_CPU_INTR",         0}, /* 15 (48)*/
    {"SDIO1_0_CPU_INTR",         0}, /* 16 (49)*/
    {"SID0_0_CPU_INTR",          INT_ENABLE_MASK}, /* 17 (50)*/
    {"SOFT_MODEM_CPU_INTR",      INT_ENABLE_MASK}, /* 18 (51)*/
    {"SVD0_0_CPU_INTR",          INT_ENABLE_MASK}, /* 19 (52)*/
    {"SYS_CPU_INTR",             INT_ENABLE_MASK}, /* 20 (53)*/
    {"SYS_AON_CPU_INTR",         INT_ENABLE_MASK}, /* 21 (54)*/
    {"SYS_PM_CPU_INTR",          0}, /* 22 (55)*/
    {"UHF_CPU_INTR",             INT_ENABLE_MASK}, /* 23 (56)*/
    {"UPG_AUX_CPU_INTR",         INT_ENABLE_MASK}, /* 24 (57)*/
    {"UPG_AUX_AON_CPU_INTR",     INT_ENABLE_MASK}, /* 25 (58)*/
    {"UPG_BSC_CPU_INTR",         INT_ENABLE_MASK}, /* 26 (59)*/
    {"UPG_BSC_AON_CPU_INTR",     INT_ENABLE_MASK}, /* 27 (60)*/
    {"UPG_MAIN_CPU_INTR",        INT_ENABLE_MASK}, /* 28 (61)*/
    {"UPG_MAIN_AON_CPU_INTR",    INT_ENABLE_MASK}, /* 29 (62)*/
    {"UPG_MC_CPU_INTR",          0}, /* 30 (63)*/
    {"UPG_SC_CPU_INTR",          INT_ENABLE_MASK}, /* 31 (64)*/
    {"UPG_SPI_CPU_INTR",         INT_ENABLE_MASK | INT_REG_WRAP}, /* 0 (65)*/
    {"UPG_TMR_CPU_INTR",         INT_ENABLE_MASK}, /* 1 (66)*/
    {"UPG_UART0_CPU_INTR",       0}, /* 2 (67)*/
    {"UPG_UART1_CPU_INTR",       0}, /* 3 (68)*/
    {"UPG_UART2_CPU_INTR",       0}, /* 4 (69)*/
    {"USB0_BRIDGE_CPU_INTR",     0}, /* 5 (70)*/
    {"USB0_EHCI_0_CPU_INTR",     0}, /* 6 (71)*/
    {"USB0_EHCI_1_CPU_INTR",     0}, /* 7 (72)*/
    {"USB0_OHCI_0_CPU_INTR",     0}, /* 8 (73)*/
    {"USB0_OHCI_1_CPU_INTR",     0}, /* 9 (74)*/
    {"USB1_BRIDGE_CPU_INTR",     0}, /* 10 (75)*/
    {"USB1_EHCI_0_CPU_INTR",     0}, /* 11 (76)*/
    {"USB1_EHCI_1_CPU_INTR ",    0}, /* 12 (77)*/
    {"USB1_OHCI_0_CPU_INTR",     0}, /* 13 (78)*/
    {"USB1_OHCI_1_CPU_INTR",     0}, /* 14 (79)*/
    {"V3D_CPU_INTR",             INT_ENABLE_MASK}, /* 15 (80)*/
    {"VEC_CPU_INTR",             INT_ENABLE_MASK}, /* 16 (81)*/
    {"VICE0_0_CPU_INTR",         INT_ENABLE_MASK}, /* 17 (82)*/
    {"VICE1_0_CPU_INTR",         INT_ENABLE_MASK}, /* 18 (83)*/
    {"WEBHIF_CPU_INTR",          0}, /* 19 (84)*/
    {"WEBHIF_IPI0_CPU_INTR",     0}, /* 20 (85)*/
    {"WEBHIF_IPI1_CPU_INTR",     0}, /* 21 (86)*/
    {"WEBHIF_STB_IPI0_CPU_INTR", 0}, /* 22 (87)*/
    {"WEBHIF_STB_IPI1_CPU_INTR", 0}, /* 23 (88)*/
    {"WEBHIF_TIMER_CPU_INTR",    0}, /* 24 (89)*/
    {"XPT_FE_CPU_INTR",          INT_ENABLE_MASK}, /* 25 (90)*/
    {"XPT_MSG_CPU_INTR",         INT_ENABLE_MASK}, /* 26 (91)*/
    {"XPT_MSG_STAT_CPU_INTR",    INT_ENABLE_MASK}, /* 27 (92)*/
    {"XPT_OVFL_CPU_INTR",        INT_ENABLE_MASK}, /* 28 (93)*/
    {"XPT_PCR_CPU_INTR",         INT_ENABLE_MASK}, /* 29 (94)*/
    {"XPT_RAV_CPU_INTR",         INT_ENABLE_MASK}, /* 30 (95)*/
    {"XPT_STATUS_CPU_INTR",      INT_ENABLE_MASK}, /* 31 (96)*/
    {"ZCPU_ACCESS_VIOL0_CPU_INTR",0 | INT_REG_WRAP}, /* 0 (97)*/
    {"ZCPU_ACCESS_VIOL1_CPU_INTR",0}, /* 1 (98)*/
    {"M2MC1_CPU_INTR",           INT_ENABLE_MASK}, /* 2 (99)*/
    {NULL,                       0}, /* 3 (100)*/
    {NULL,                       0}, /* 4 (101)*/
    {NULL,                       0}, /* 5 (102)*/
    {NULL,                       0}, /* 6 (103)*/
    {NULL,                       0}, /* 7 (104)*/
    {NULL,                       0}, /* 8 (105)*/
    {NULL,                       0}, /* 9 (106)*/
    {NULL,                       0}, /* 10 (107)*/
    {NULL,                       0}, /* 11 (108)*/
    {NULL,                       0}, /* 12 (109)*/
    {NULL,                       0}, /* 13 (110)*/
    {NULL,                       0}, /* 14 (111)*/
    {NULL,                       0}, /* 15 (112)*/
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
#else
/* NEXUS_WEBCPU client */
s_InteruptTable interruptName7435[] = {
    {NULL, 0},                   /* Linux IRQ #'s are offset by 1 */
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"EXT_IRQ_05_CPU_INTR",      0}, /* 16 (17)*/
    {"GENET_0_A_CPU_INTR",       0}, /* 17 (18)*/
    {"GENET_0_B_CPU_INTR",       0}, /* 18 (19)*/
    {"GENET_1_A_CPU_INTR",       0}, /* 19 (20)*/
    {"GENET_1_B_CPU_INTR",       0}, /* 20 (21)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"HIF_CPU_INTR",             0}, /* 24 (25)*/
    {"HIF_SPI_CPU_INTR",         0}, /* 25 (26)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"MOCA_CPU_INTR",            0 | INT_REG_WRAP},/* 0 (33)*/
    {NULL,                       0},
    {"PCIE_ERR_ATTN_CPU_INTR",   0}, /* 2 (35)*/
    {"PCIE_INTA_CPU_INTR",       0}, /* 3 (36)*/
    {"PCIE_INTB_CPU_INTR",       0}, /* 4 (37)*/
    {"PCIE_INTC_CPU_INTR",       0}, /* 5 (38)*/
    {"PCIE_INTD_CPU_INTR",       0}, /* 6 (39)*/
    {"PCIE_INTR_CPU_INTR",       0}, /* 7 (40)*/
    {"PCIE_NMI_CPU_INTR",        0}, /* 8 (41)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"SATA_AHCI_CPU_INTR",       0}, /* 13 (46)*/
    {"SATA_GRB_CPU_INTR",        0}, /* 14 (47)*/
    {"SDIO0_0_CPU_INTR",         0}, /* 15 (48)*/
    {"SDIO1_0_CPU_INTR",         0}, /* 16 (49)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"SYS_PM_CPU_INTR",          0}, /* 22 (55)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"UPG_MC_CPU_INTR",          0}, /* 30 (63)*/
    {NULL,                       0},
    {"UPG_SPI_CPU_INTR",         0 | INT_REG_WRAP}, /* 0 (65)*/
    {NULL,                       0},
    {"UPG_UART0_CPU_INTR",       0}, /* 2 (67)*/
    {"UPG_UART1_CPU_INTR",       0}, /* 3 (68)*/
    {"UPG_UART2_CPU_INTR",       0}, /* 4 (69)*/
    {"USB0_BRIDGE_CPU_INTR",     0}, /* 5 (70)*/
    {"USB0_EHCI_0_CPU_INTR",     0}, /* 6 (71)*/
    {"USB0_EHCI_1_CPU_INTR",     0}, /* 7 (72)*/
    {"USB0_OHCI_0_CPU_INTR",     0}, /* 8 (73)*/
    {"USB0_OHCI_1_CPU_INTR",     0}, /* 9 (74)*/
    {"USB1_BRIDGE_CPU_INTR",     0}, /* 10 (75)*/
    {"USB1_EHCI_0_CPU_INTR",     0}, /* 11 (76)*/
    {"USB1_EHCI_1_CPU_INTR ",    0}, /* 12 (77)*/
    {"USB1_OHCI_0_CPU_INTR",     0}, /* 13 (78)*/
    {"USB1_OHCI_1_CPU_INTR",     0}, /* 14 (79)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"WEBHIF_CPU_INTR",          0}, /* 19 (84)*/
    {"WEBHIF_IPI0_CPU_INTR",     0}, /* 20 (85)*/
    {"WEBHIF_IPI1_CPU_INTR",     0}, /* 21 (86)*/
    {"WEBHIF_STB_IPI0_CPU_INTR", 0}, /* 22 (87)*/
    {"WEBHIF_STB_IPI1_CPU_INTR", 0}, /* 23 (88)*/
    {"WEBHIF_TIMER_CPU_INTR",    0}, /* 24 (89)*/
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {NULL,                       0},
    {"ZCPU_ACCESS_VIOL0_CPU_INTR",0 | INT_REG_WRAP}, /* 0 (97)*/
    {"ZCPU_ACCESS_VIOL1_CPU_INTR",0}, /* 1 (98)*/
    /* CPU1 uses both M2MC1 */
    {"M2MC1_CPU_INTR",           INT_ENABLE_MASK}, /* 2 (99)*/
    {NULL,                       0}, /* 3 (100)*/
    {NULL,                       0}, /* 4 (101)*/
    {NULL,                       0}, /* 5 (102)*/
    {NULL,                       0}, /* 6 (103)*/
    {NULL,                       0}, /* 7 (104)*/
    {NULL,                       0}, /* 8 (105)*/
    {NULL,                       0}, /* 9 (106)*/
    {NULL,                       0}, /* 10 (107)*/
    {NULL,                       0}, /* 11 (108)*/
    {NULL,                       0}, /* 12 (109)*/
    {NULL,                       0}, /* 13 (110)*/
    {NULL,                       0}, /* 14 (111)*/
    {NULL,                       0}, /* 15 (112)*/
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

s_ChipConfig  g_sChipConfig ={ "7435",interruptName7435, 0xb041b500, sizeof(interruptName7435)/sizeof(s_InteruptTable),NUM_INTC_WORDS};
