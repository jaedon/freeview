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
 * $brcm_Workfile: bchp_pcie_ep_cfg_pm.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/7/11 3:25p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Dec  6 18:45:28 2011
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_pcie_ep_cfg_pm.h $
 * 
 * Hydra_Software_Devel/2   12/7/11 3:25p mward
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_PCIE_EP_CFG_PM_H__
#define BCHP_PCIE_EP_CFG_PM_H__

/***************************************************************************
 *PCIE_EP_CFG_PM
 ***************************************************************************/
#define BCHP_PCIE_EP_CFG_PM_PM_CAP               0x00412048 /* pm_cap */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR               0x0041204c /* pm_csr */

/***************************************************************************
 *PM_CAP - pm_cap
 ***************************************************************************/
/* PCIE_EP_CFG_PM :: PM_CAP :: PME_IN_D3_COLD [31:31] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D3_COLD_MASK             0x80000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D3_COLD_SHIFT            31
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D3_COLD_DEFAULT          0x00000001

/* PCIE_EP_CFG_PM :: PM_CAP :: PME_IN_D3_HOT [30:30] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D3_HOT_MASK              0x40000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D3_HOT_SHIFT             30
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D3_HOT_DEFAULT           0x00000001

/* PCIE_EP_CFG_PM :: PM_CAP :: PME_IN_D2 [29:29] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D2_MASK                  0x20000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D2_SHIFT                 29
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D2_DEFAULT               0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: PME_IN_D1 [28:28] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D1_MASK                  0x10000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D1_SHIFT                 28
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D1_DEFAULT               0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: PME_IN_D0 [27:27] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D0_MASK                  0x08000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D0_SHIFT                 27
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PME_IN_D0_DEFAULT               0x00000001

/* PCIE_EP_CFG_PM :: PM_CAP :: D2_SUPPORT [26:26] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_D2_SUPPORT_MASK                 0x04000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_D2_SUPPORT_SHIFT                26
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_D2_SUPPORT_DEFAULT              0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: D1_SUPPORT [25:25] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_D1_SUPPORT_MASK                 0x02000000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_D1_SUPPORT_SHIFT                25
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_D1_SUPPORT_DEFAULT              0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: AUX_CURRENT [24:22] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_AUX_CURRENT_MASK                0x01c00000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_AUX_CURRENT_SHIFT               22
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_AUX_CURRENT_DEFAULT             0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: DSI [21:21] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_DSI_MASK                        0x00200000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_DSI_SHIFT                       21
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_DSI_DEFAULT                     0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: RESERVED [20:20] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_RESERVED_MASK                   0x00100000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_RESERVED_SHIFT                  20
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_RESERVED_DEFAULT                0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: CLOCK [19:19] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_CLOCK_MASK                      0x00080000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_CLOCK_SHIFT                     19
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_CLOCK_DEFAULT                   0x00000000

/* PCIE_EP_CFG_PM :: PM_CAP :: RESERVED0 [18:18] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_RESERVED0_MASK                  0x00040000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_RESERVED0_SHIFT                 18

/* PCIE_EP_CFG_PM :: PM_CAP :: VERSION [17:16] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_VERSION_MASK                    0x00030000
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_VERSION_SHIFT                   16
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_VERSION_DEFAULT                 0x00000003

/* PCIE_EP_CFG_PM :: PM_CAP :: PM_NEXT_CAP_PTR [15:08] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PM_NEXT_CAP_PTR_MASK            0x0000ff00
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PM_NEXT_CAP_PTR_SHIFT           8
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PM_NEXT_CAP_PTR_DEFAULT         0x00000050

/* PCIE_EP_CFG_PM :: PM_CAP :: PM_CAP_ID [07:00] */
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PM_CAP_ID_MASK                  0x000000ff
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PM_CAP_ID_SHIFT                 0
#define BCHP_PCIE_EP_CFG_PM_PM_CAP_PM_CAP_ID_DEFAULT               0x00000001

/***************************************************************************
 *PM_CSR - pm_csr
 ***************************************************************************/
/* PCIE_EP_CFG_PM :: PM_CSR :: PM_DATA [31:24] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PM_DATA_MASK                    0xff000000
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PM_DATA_SHIFT                   24
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PM_DATA_DEFAULT                 0x00000000

/* PCIE_EP_CFG_PM :: PM_CSR :: PM_CSR_BSE [23:16] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PM_CSR_BSE_MASK                 0x00ff0000
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PM_CSR_BSE_SHIFT                16
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PM_CSR_BSE_DEFAULT              0x00000000

/* PCIE_EP_CFG_PM :: PM_CSR :: PME_STATUS [15:15] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PME_STATUS_MASK                 0x00008000
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PME_STATUS_SHIFT                15
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PME_STATUS_DEFAULT              0x00000000

/* PCIE_EP_CFG_PM :: PM_CSR :: DATA_SCALE [14:13] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_DATA_SCALE_MASK                 0x00006000
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_DATA_SCALE_SHIFT                13
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_DATA_SCALE_DEFAULT              0x00000001

/* PCIE_EP_CFG_PM :: PM_CSR :: DATA_SEL [12:09] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_DATA_SEL_MASK                   0x00001e00
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_DATA_SEL_SHIFT                  9
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_DATA_SEL_DEFAULT                0x00000000

/* PCIE_EP_CFG_PM :: PM_CSR :: PME_ENABLE [08:08] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PME_ENABLE_MASK                 0x00000100
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PME_ENABLE_SHIFT                8
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_PME_ENABLE_DEFAULT              0x00000001

/* PCIE_EP_CFG_PM :: PM_CSR :: RESERVED1 [07:04] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_RESERVED1_MASK                  0x000000f0
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_RESERVED1_SHIFT                 4
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_RESERVED1_DEFAULT               0x00000000

/* PCIE_EP_CFG_PM :: PM_CSR :: NO_SOFT_RESET [03:03] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_NO_SOFT_RESET_MASK              0x00000008
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_NO_SOFT_RESET_SHIFT             3
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_NO_SOFT_RESET_DEFAULT           0x00000001

/* PCIE_EP_CFG_PM :: PM_CSR :: RESERVED0 [02:02] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_RESERVED0_MASK                  0x00000004
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_RESERVED0_SHIFT                 2
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_RESERVED0_DEFAULT               0x00000000

/* PCIE_EP_CFG_PM :: PM_CSR :: STATE [01:00] */
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_STATE_MASK                      0x00000003
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_STATE_SHIFT                     0
#define BCHP_PCIE_EP_CFG_PM_PM_CSR_STATE_DEFAULT                   0x00000000

#endif /* #ifndef BCHP_PCIE_EP_CFG_PM_H__ */

/* End of File */
