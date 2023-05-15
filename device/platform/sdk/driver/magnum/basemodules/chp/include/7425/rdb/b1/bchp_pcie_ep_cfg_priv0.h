/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_pcie_ep_cfg_priv0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/28/11 12:04a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 17:37:35 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_pcie_ep_cfg_priv0.h $
 * 
 * Hydra_Software_Devel/1   10/28/11 12:04a vanessah
 * SW7425-1620: add 7425 B0 rdb header file
 *
 ***************************************************************************/

#ifndef BCHP_PCIE_EP_CFG_PRIV0_H__
#define BCHP_PCIE_EP_CFG_PRIV0_H__

/***************************************************************************
 *PCIE_EP_CFG_PRIV0
 ***************************************************************************/
#define BCHP_PCIE_EP_CFG_PRIV0_UNUSED_F          0x00412404 /* unused_f */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2          0x00412408 /* config_2 */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3          0x0041240c /* config_3 */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A         0x00412410 /* pm_data_a */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B         0x00412414 /* pm_data_b */
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE 0x00412418 /* pci_higher_bar_size */

/***************************************************************************
 *UNUSED_F - unused_f
 ***************************************************************************/
/* PCIE_EP_CFG_PRIV0 :: UNUSED_F :: RESERVED [31:00] */
#define BCHP_PCIE_EP_CFG_PRIV0_UNUSED_F_RESERVED_MASK              0xffffffff
#define BCHP_PCIE_EP_CFG_PRIV0_UNUSED_F_RESERVED_SHIFT             0

/***************************************************************************
 *CONFIG_2 - config_2
 ***************************************************************************/
/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: RESERVED0 [31:17] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_RESERVED0_MASK             0xfffe0000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_RESERVED0_SHIFT            17
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_RESERVED0_DEFAULT          0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: BAR_PREFETCH [16:16] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR_PREFETCH_MASK          0x00010000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR_PREFETCH_SHIFT         16
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR_PREFETCH_DEFAULT       1

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: ROM_BAR_SIZE [15:08] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_ROM_BAR_SIZE_MASK          0x0000ff00
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_ROM_BAR_SIZE_SHIFT         8
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_ROM_BAR_SIZE_DEFAULT       0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: FIRST_CFG_DONE [07:07] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_FIRST_CFG_DONE_MASK        0x00000080
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_FIRST_CFG_DONE_SHIFT       7
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_FIRST_CFG_DONE_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: CFG_CYCLE_RETRY [06:06] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_CFG_CYCLE_RETRY_MASK       0x00000040
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_CFG_CYCLE_RETRY_SHIFT      6
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_CFG_CYCLE_RETRY_DEFAULT    1

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: EXP_ROM_RETRY [05:05] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_EXP_ROM_RETRY_MASK         0x00000020
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_EXP_ROM_RETRY_SHIFT        5
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_EXP_ROM_RETRY_DEFAULT      0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: BAR1_64ENA [04:04] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR1_64ENA_MASK            0x00000010
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR1_64ENA_SHIFT           4
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR1_64ENA_DEFAULT         1

/* PCIE_EP_CFG_PRIV0 :: CONFIG_2 :: BAR1_SIZE [03:00] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR1_SIZE_MASK             0x0000000f
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR1_SIZE_SHIFT            0
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_2_BAR1_SIZE_DEFAULT          9

/***************************************************************************
 *CONFIG_3 - config_3
 ***************************************************************************/
/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: PCI_POWER [31:31] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PCI_POWER_MASK             0x80000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PCI_POWER_SHIFT            31
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PCI_POWER_DEFAULT          0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: VAUX_PRESET [30:30] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_VAUX_PRESET_MASK           0x40000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_VAUX_PRESET_SHIFT          30
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_VAUX_PRESET_DEFAULT        0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: RESERVED1 [29:29] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_RESERVED1_MASK             0x20000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_RESERVED1_SHIFT            29

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: PM_STATE [28:27] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PM_STATE_MASK              0x18000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PM_STATE_SHIFT             27
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PM_STATE_DEFAULT           0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: PME_ENABLE [26:26] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PME_ENABLE_MASK            0x04000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PME_ENABLE_SHIFT           26
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PME_ENABLE_DEFAULT         1

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: PME_STATUS [25:25] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PME_STATUS_MASK            0x02000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PME_STATUS_SHIFT           25
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_PME_STATUS_DEFAULT         0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: FORCE_PME [24:24] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_FORCE_PME_MASK             0x01000000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_FORCE_PME_SHIFT            24
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_FORCE_PME_DEFAULT          0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: RESERVED0 [23:16] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_RESERVED0_MASK             0x00ff0000
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_RESERVED0_SHIFT            16

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: REG_STICKY_BYTE [15:08] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_REG_STICKY_BYTE_MASK       0x0000ff00
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_REG_STICKY_BYTE_SHIFT      8
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_REG_STICKY_BYTE_DEFAULT    0

/* PCIE_EP_CFG_PRIV0 :: CONFIG_3 :: STICKY_BYTE [07:00] */
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_STICKY_BYTE_MASK           0x000000ff
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_STICKY_BYTE_SHIFT          0
#define BCHP_PCIE_EP_CFG_PRIV0_CONFIG_3_STICKY_BYTE_DEFAULT        0

/***************************************************************************
 *PM_DATA_A - pm_data_a
 ***************************************************************************/
/* PCIE_EP_CFG_PRIV0 :: PM_DATA_A :: PM_DATA_3_PRG [31:24] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_3_PRG_MASK        0xff000000
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_3_PRG_SHIFT       24
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_3_PRG_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: PM_DATA_A :: PM_DATA_2_PRG [23:16] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_2_PRG_MASK        0x00ff0000
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_2_PRG_SHIFT       16
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_2_PRG_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: PM_DATA_A :: PM_DATA_1_PRG [15:08] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_1_PRG_MASK        0x0000ff00
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_1_PRG_SHIFT       8
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_1_PRG_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: PM_DATA_A :: PM_DATA_0_PRG [07:00] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_0_PRG_MASK        0x000000ff
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_0_PRG_SHIFT       0
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_A_PM_DATA_0_PRG_DEFAULT     0

/***************************************************************************
 *PM_DATA_B - pm_data_b
 ***************************************************************************/
/* PCIE_EP_CFG_PRIV0 :: PM_DATA_B :: PM_DATA_7_PRG [31:24] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_7_PRG_MASK        0xff000000
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_7_PRG_SHIFT       24
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_7_PRG_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: PM_DATA_B :: PM_DATA_6_PRG [23:16] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_6_PRG_MASK        0x00ff0000
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_6_PRG_SHIFT       16
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_6_PRG_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: PM_DATA_B :: PM_DATA_5_PRG [15:08] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_5_PRG_MASK        0x0000ff00
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_5_PRG_SHIFT       8
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_5_PRG_DEFAULT     0

/* PCIE_EP_CFG_PRIV0 :: PM_DATA_B :: PM_DATA_4_PRG [07:00] */
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_4_PRG_MASK        0x000000ff
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_4_PRG_SHIFT       0
#define BCHP_PCIE_EP_CFG_PRIV0_PM_DATA_B_PM_DATA_4_PRG_DEFAULT     0

/***************************************************************************
 *PCI_HIGHER_BAR_SIZE - pci_higher_bar_size
 ***************************************************************************/
/* PCIE_EP_CFG_PRIV0 :: PCI_HIGHER_BAR_SIZE :: RESERVED0 [31:12] */
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_RESERVED0_MASK  0xfffff000
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_RESERVED0_SHIFT 12

/* PCIE_EP_CFG_PRIV0 :: PCI_HIGHER_BAR_SIZE :: BAR3_SIZE_HIEXT [11:08] */
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR3_SIZE_HIEXT_MASK 0x00000f00
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR3_SIZE_HIEXT_SHIFT 8
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR3_SIZE_HIEXT_DEFAULT 0

/* PCIE_EP_CFG_PRIV0 :: PCI_HIGHER_BAR_SIZE :: BAR2_SIZE_HIEXT [07:04] */
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR2_SIZE_HIEXT_MASK 0x000000f0
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR2_SIZE_HIEXT_SHIFT 4
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR2_SIZE_HIEXT_DEFAULT 0

/* PCIE_EP_CFG_PRIV0 :: PCI_HIGHER_BAR_SIZE :: BAR1_SIZE_HIEXT [03:00] */
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR1_SIZE_HIEXT_MASK 0x0000000f
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR1_SIZE_HIEXT_SHIFT 0
#define BCHP_PCIE_EP_CFG_PRIV0_PCI_HIGHER_BAR_SIZE_BAR1_SIZE_HIEXT_DEFAULT 0

#endif /* #ifndef BCHP_PCIE_EP_CFG_PRIV0_H__ */

/* End of File */