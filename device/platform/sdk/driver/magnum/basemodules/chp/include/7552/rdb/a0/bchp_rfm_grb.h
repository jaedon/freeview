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
 * $brcm_Workfile: bchp_rfm_grb.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/18/11 4:29p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon May 16 20:57:47 2011
 *                 MD5 Checksum         f5f09b2bf7ad40890d2e5dc57d4789b6
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/a0/bchp_rfm_grb.h $
 * 
 * Hydra_Software_Devel/2   5/18/11 4:29p xhuang
 * SW7552-2: update with central RDB
 *
 ***************************************************************************/

#ifndef BCHP_RFM_GRB_H__
#define BCHP_RFM_GRB_H__

/***************************************************************************
 *RFM_GRB - RFM GISB Bridge Registers
 ***************************************************************************/
#define BCHP_RFM_GRB_REVISION                    0x00b01000 /* GR Bridge Revision */
#define BCHP_RFM_GRB_CTRL                        0x00b01004 /* GR Bridge Control Register */
#define BCHP_RFM_GRB_SW_INIT_0                   0x00b01008 /* GR Bridge Software Init 0 Register */
#define BCHP_RFM_GRB_SW_INIT_1                   0x00b0100c /* GR Bridge Software Init 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* RFM_GRB :: REVISION :: reserved0 [31:16] */
#define BCHP_RFM_GRB_REVISION_reserved0_MASK                       0xffff0000
#define BCHP_RFM_GRB_REVISION_reserved0_SHIFT                      16

/* RFM_GRB :: REVISION :: MAJOR [15:08] */
#define BCHP_RFM_GRB_REVISION_MAJOR_MASK                           0x0000ff00
#define BCHP_RFM_GRB_REVISION_MAJOR_SHIFT                          8
#define BCHP_RFM_GRB_REVISION_MAJOR_DEFAULT                        2

/* RFM_GRB :: REVISION :: MINOR [07:00] */
#define BCHP_RFM_GRB_REVISION_MINOR_MASK                           0x000000ff
#define BCHP_RFM_GRB_REVISION_MINOR_SHIFT                          0
#define BCHP_RFM_GRB_REVISION_MINOR_DEFAULT                        0

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* RFM_GRB :: CTRL :: reserved0 [31:01] */
#define BCHP_RFM_GRB_CTRL_reserved0_MASK                           0xfffffffe
#define BCHP_RFM_GRB_CTRL_reserved0_SHIFT                          1

/* RFM_GRB :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_MASK                     0x00000001
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_SHIFT                    0
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_DEFAULT                  0
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_INTR_DISABLE             0
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_INTR_ENABLE              1

/***************************************************************************
 *SW_INIT_0 - GR Bridge Software Init 0 Register
 ***************************************************************************/
/* RFM_GRB :: SW_INIT_0 :: reserved0 [31:03] */
#define BCHP_RFM_GRB_SW_INIT_0_reserved0_MASK                      0xfffffff8
#define BCHP_RFM_GRB_SW_INIT_0_reserved0_SHIFT                     3

/* RFM_GRB :: SW_INIT_0 :: YOUR_NAME02_SW_INIT [02:02] */
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME02_SW_INIT_MASK            0x00000004
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME02_SW_INIT_SHIFT           2
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME02_SW_INIT_DEFAULT         0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME02_SW_INIT_DEASSERT        0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME02_SW_INIT_ASSERT          1

/* RFM_GRB :: SW_INIT_0 :: YOUR_NAME01_SW_INIT [01:01] */
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME01_SW_INIT_MASK            0x00000002
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME01_SW_INIT_SHIFT           1
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME01_SW_INIT_DEFAULT         0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME01_SW_INIT_DEASSERT        0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME01_SW_INIT_ASSERT          1

/* RFM_GRB :: SW_INIT_0 :: YOUR_NAME00_SW_INIT [00:00] */
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME00_SW_INIT_MASK            0x00000001
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME00_SW_INIT_SHIFT           0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME00_SW_INIT_DEFAULT         0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME00_SW_INIT_DEASSERT        0
#define BCHP_RFM_GRB_SW_INIT_0_YOUR_NAME00_SW_INIT_ASSERT          1

/***************************************************************************
 *SW_INIT_1 - GR Bridge Software Init 1 Register
 ***************************************************************************/
/* RFM_GRB :: SW_INIT_1 :: reserved0 [31:03] */
#define BCHP_RFM_GRB_SW_INIT_1_reserved0_MASK                      0xfffffff8
#define BCHP_RFM_GRB_SW_INIT_1_reserved0_SHIFT                     3

/* RFM_GRB :: SW_INIT_1 :: YOUR_NAME02_SW_INIT [02:02] */
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME02_SW_INIT_MASK            0x00000004
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME02_SW_INIT_SHIFT           2
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME02_SW_INIT_DEFAULT         1
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME02_SW_INIT_DEASSERT        0
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME02_SW_INIT_ASSERT          1

/* RFM_GRB :: SW_INIT_1 :: YOUR_NAME01_SW_INIT [01:01] */
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME01_SW_INIT_MASK            0x00000002
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME01_SW_INIT_SHIFT           1
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME01_SW_INIT_DEFAULT         1
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME01_SW_INIT_DEASSERT        0
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME01_SW_INIT_ASSERT          1

/* RFM_GRB :: SW_INIT_1 :: YOUR_NAME00_SW_INIT [00:00] */
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME00_SW_INIT_MASK            0x00000001
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME00_SW_INIT_SHIFT           0
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME00_SW_INIT_DEFAULT         1
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME00_SW_INIT_DEASSERT        0
#define BCHP_RFM_GRB_SW_INIT_1_YOUR_NAME00_SW_INIT_ASSERT          1

#endif /* #ifndef BCHP_RFM_GRB_H__ */

/* End of File */