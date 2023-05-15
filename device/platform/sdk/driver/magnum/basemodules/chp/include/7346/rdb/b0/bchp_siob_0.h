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
 * $brcm_Workfile: bchp_siob_0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/12/11 3:43p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Dec 12 12:00:26 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_siob_0.h $
 * 
 * Hydra_Software_Devel/2   12/12/11 3:43p pntruong
 * SW7346-143: Resynced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SIOB_0_H__
#define BCHP_SIOB_0_H__

/***************************************************************************
 *SIOB_0 - BVN_MADR_TOP SIOB Control Registers 0
 ***************************************************************************/
#define BCHP_SIOB_0_DCXS_CFG                     0x00660200 /* DCXS configuration */
#define BCHP_SIOB_0_TEST_PORT_CONTROL            0x006602f8 /* Test port control register */
#define BCHP_SIOB_0_TEST_PORT_DATA               0x006602fc /* Test port data register */

/***************************************************************************
 *DCXS_CFG - DCXS configuration
 ***************************************************************************/
/* SIOB_0 :: DCXS_CFG :: ENABLE [31:31] */
#define BCHP_SIOB_0_DCXS_CFG_ENABLE_MASK                           0x80000000
#define BCHP_SIOB_0_DCXS_CFG_ENABLE_SHIFT                          31
#define BCHP_SIOB_0_DCXS_CFG_ENABLE_DEFAULT                        0x00000000
#define BCHP_SIOB_0_DCXS_CFG_ENABLE_Enable                         1
#define BCHP_SIOB_0_DCXS_CFG_ENABLE_Disable                        0

/* SIOB_0 :: DCXS_CFG :: reserved0 [30:05] */
#define BCHP_SIOB_0_DCXS_CFG_reserved0_MASK                        0x7fffffe0
#define BCHP_SIOB_0_DCXS_CFG_reserved0_SHIFT                       5

/* SIOB_0 :: DCXS_CFG :: APPLY_QERR [04:03] */
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_MASK                       0x00000018
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_SHIFT                      3
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_DEFAULT                    0x00000000
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_Reserved                   3
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_Apply_Half_Qerr            2
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_Apply_Qerr                 1
#define BCHP_SIOB_0_DCXS_CFG_APPLY_QERR_No_Apply                   0

/* SIOB_0 :: DCXS_CFG :: FIXED_RATE [02:02] */
#define BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_MASK                       0x00000004
#define BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_SHIFT                      2
#define BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_DEFAULT                    0x00000000
#define BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_Fixed                      1
#define BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_Variable                   0

/* SIOB_0 :: DCXS_CFG :: COMPRESSION [01:00] */
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_MASK                      0x00000003
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_SHIFT                     0
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_DEFAULT                   0x00000001
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_Unused                    3
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_BPP_11                    2
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_BPP_9                     1
#define BCHP_SIOB_0_DCXS_CFG_COMPRESSION_Reserved                  0

/***************************************************************************
 *TEST_PORT_CONTROL - Test port control register
 ***************************************************************************/
/* SIOB_0 :: TEST_PORT_CONTROL :: reserved0 [31:03] */
#define BCHP_SIOB_0_TEST_PORT_CONTROL_reserved0_MASK               0xfffffff8
#define BCHP_SIOB_0_TEST_PORT_CONTROL_reserved0_SHIFT              3

/* SIOB_0 :: TEST_PORT_CONTROL :: TP_ADDR [02:00] */
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_MASK                 0x00000007
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SHIFT                0
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_DEFAULT              0x00000000
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL0            0
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL1            1
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL2            2
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL3            3
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL4            4
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL5            5
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL6            6
#define BCHP_SIOB_0_TEST_PORT_CONTROL_TP_ADDR_SIOB_SEL7            7

/***************************************************************************
 *TEST_PORT_DATA - Test port data register
 ***************************************************************************/
/* union - case SIOB_TP_SEL0 [31:00] */
/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL0 :: reserved0 [31:28] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_reserved0_MASK     0xf0000000
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_reserved0_SHIFT    28

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL0 :: FEEDER_FIFO_CTRL [27:24] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_FIFO_CTRL_MASK 0x0f000000
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_FIFO_CTRL_SHIFT 24

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL0 :: FEEDER_QM [23:16] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_QM_MASK     0x00ff0000
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_QM_SHIFT    16

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL0 :: FEEDER_G [15:12] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_G_MASK      0x0000f000
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_G_SHIFT     12

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL0 :: FEEDER_J [11:08] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_J_MASK      0x00000f00
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_FEEDER_J_SHIFT     8

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL0 :: reserved1 [07:00] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_reserved1_MASK     0x000000ff
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL0_reserved1_SHIFT    0

/* union - case SIOB_TP_SEL1 [31:00] */
/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL1 :: reserved0 [31:08] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL1_reserved0_MASK     0xffffff00
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL1_reserved0_SHIFT    8

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL1 :: CAPTURE_B [07:04] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL1_CAPTURE_B_MASK     0x000000f0
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL1_CAPTURE_B_SHIFT    4

/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL1 :: CAPTURE_QM [03:00] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL1_CAPTURE_QM_MASK    0x0000000f
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL1_CAPTURE_QM_SHIFT   0

/* union - case SIOB_TP_SEL2 [31:00] */
/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL2 :: DCD_J_DEBUG [31:00] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL2_DCD_J_DEBUG_MASK   0xffffffff
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL2_DCD_J_DEBUG_SHIFT  0

/* union - case SIOB_TP_SEL3 [31:00] */
/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL3 :: DCD_G_DEBUG [31:00] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL3_DCD_G_DEBUG_MASK   0xffffffff
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL3_DCD_G_DEBUG_SHIFT  0

/* union - case SIOB_TP_SEL4 [31:00] */
/* SIOB_0 :: TEST_PORT_DATA :: SIOB_TP_SEL4 :: DCE_B_DEBUG [31:00] */
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL4_DCE_B_DEBUG_MASK   0xffffffff
#define BCHP_SIOB_0_TEST_PORT_DATA_SIOB_TP_SEL4_DCE_B_DEBUG_SHIFT  0

#endif /* #ifndef BCHP_SIOB_0_H__ */

/* End of File */