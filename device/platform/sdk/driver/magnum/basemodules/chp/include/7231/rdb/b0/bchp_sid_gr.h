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
 * $brcm_Workfile: bchp_sid_gr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/11 4:57p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 22 16:12:00 2011
 *                 MD5 Checksum         f1fe9dd101680af6476d6b0b4e1d855e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b0/bchp_sid_gr.h $
 * 
 * Hydra_Software_Devel/1   6/22/11 4:57p pntruong
 * SW7231-196: Synced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SID_GR_H__
#define BCHP_SID_GR_H__

/***************************************************************************
 *SID_GR - SID GR Bridge Registers
 ***************************************************************************/
#define BCHP_SID_GR_REVISION                     0x00560000 /* GR Bridge Revision */
#define BCHP_SID_GR_CTRL                         0x00560004 /* GR Bridge Control Register */
#define BCHP_SID_GR_SW_INIT_0                    0x00560008 /* GR Bridge Software Init 0 Register */
#define BCHP_SID_GR_SW_INIT_1                    0x0056000c /* GR Bridge Software Init 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* SID_GR :: REVISION :: reserved0 [31:16] */
#define BCHP_SID_GR_REVISION_reserved0_MASK                        0xffff0000
#define BCHP_SID_GR_REVISION_reserved0_SHIFT                       16

/* SID_GR :: REVISION :: MAJOR [15:08] */
#define BCHP_SID_GR_REVISION_MAJOR_MASK                            0x0000ff00
#define BCHP_SID_GR_REVISION_MAJOR_SHIFT                           8
#define BCHP_SID_GR_REVISION_MAJOR_DEFAULT                         2

/* SID_GR :: REVISION :: MINOR [07:00] */
#define BCHP_SID_GR_REVISION_MINOR_MASK                            0x000000ff
#define BCHP_SID_GR_REVISION_MINOR_SHIFT                           0
#define BCHP_SID_GR_REVISION_MINOR_DEFAULT                         0

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* SID_GR :: CTRL :: reserved0 [31:01] */
#define BCHP_SID_GR_CTRL_reserved0_MASK                            0xfffffffe
#define BCHP_SID_GR_CTRL_reserved0_SHIFT                           1

/* SID_GR :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_SID_GR_CTRL_gisb_error_intr_MASK                      0x00000001
#define BCHP_SID_GR_CTRL_gisb_error_intr_SHIFT                     0
#define BCHP_SID_GR_CTRL_gisb_error_intr_DEFAULT                   0
#define BCHP_SID_GR_CTRL_gisb_error_intr_INTR_DISABLE              0
#define BCHP_SID_GR_CTRL_gisb_error_intr_INTR_ENABLE               1

/***************************************************************************
 *SW_INIT_0 - GR Bridge Software Init 0 Register
 ***************************************************************************/
/* SID_GR :: SW_INIT_0 :: reserved0 [31:01] */
#define BCHP_SID_GR_SW_INIT_0_reserved0_MASK                       0xfffffffe
#define BCHP_SID_GR_SW_INIT_0_reserved0_SHIFT                      1

/* SID_GR :: SW_INIT_0 :: SID_CLK_108_SW_INIT [00:00] */
#define BCHP_SID_GR_SW_INIT_0_SID_CLK_108_SW_INIT_MASK             0x00000001
#define BCHP_SID_GR_SW_INIT_0_SID_CLK_108_SW_INIT_SHIFT            0
#define BCHP_SID_GR_SW_INIT_0_SID_CLK_108_SW_INIT_DEFAULT          0
#define BCHP_SID_GR_SW_INIT_0_SID_CLK_108_SW_INIT_DEASSERT         0
#define BCHP_SID_GR_SW_INIT_0_SID_CLK_108_SW_INIT_ASSERT           1

/***************************************************************************
 *SW_INIT_1 - GR Bridge Software Init 1 Register
 ***************************************************************************/
/* SID_GR :: SW_INIT_1 :: reserved0 [31:01] */
#define BCHP_SID_GR_SW_INIT_1_reserved0_MASK                       0xfffffffe
#define BCHP_SID_GR_SW_INIT_1_reserved0_SHIFT                      1

/* SID_GR :: SW_INIT_1 :: YOUR_NAME00_SW_INIT [00:00] */
#define BCHP_SID_GR_SW_INIT_1_YOUR_NAME00_SW_INIT_MASK             0x00000001
#define BCHP_SID_GR_SW_INIT_1_YOUR_NAME00_SW_INIT_SHIFT            0
#define BCHP_SID_GR_SW_INIT_1_YOUR_NAME00_SW_INIT_DEFAULT          1
#define BCHP_SID_GR_SW_INIT_1_YOUR_NAME00_SW_INIT_DEASSERT         0
#define BCHP_SID_GR_SW_INIT_1_YOUR_NAME00_SW_INIT_ASSERT           1

#endif /* #ifndef BCHP_SID_GR_H__ */

/* End of File */