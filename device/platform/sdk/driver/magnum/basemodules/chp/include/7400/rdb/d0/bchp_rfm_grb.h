/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_rfm_grb.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/23/07 4:00p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jul 23 11:55:59 2007
 *                 MD5 Checksum         61f9c4d8dcdcd06017506dddbf23f434
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008004
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_rfm_grb.h $
 * 
 * Hydra_Software_Devel/1   7/23/07 4:00p maivu
 * PR 32842: Initial revision
 *
 ***************************************************************************/

#ifndef BCHP_RFM_GRB_H__
#define BCHP_RFM_GRB_H__

/***************************************************************************
 *RFM_GRB - RFM GISB Bridge Registers
 ***************************************************************************/
#define BCHP_RFM_GRB_REVISION                    0x00281200 /* GR Bridge Revision */
#define BCHP_RFM_GRB_CTRL                        0x00281204 /* GR Bridge Control Register */
#define BCHP_RFM_GRB_SW_RESET_0                  0x00281208 /* GR Bridge Software Reset 0 Register */
#define BCHP_RFM_GRB_SW_RESET_1                  0x0028120c /* GR Bridge Software Reset 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* RFM_GRB :: REVISION :: reserved0 [31:16] */
#define BCHP_RFM_GRB_REVISION_reserved0_MASK                       0xffff0000
#define BCHP_RFM_GRB_REVISION_reserved0_SHIFT                      16

/* RFM_GRB :: REVISION :: MAJOR [15:08] */
#define BCHP_RFM_GRB_REVISION_MAJOR_MASK                           0x0000ff00
#define BCHP_RFM_GRB_REVISION_MAJOR_SHIFT                          8

/* RFM_GRB :: REVISION :: MINOR [07:00] */
#define BCHP_RFM_GRB_REVISION_MINOR_MASK                           0x000000ff
#define BCHP_RFM_GRB_REVISION_MINOR_SHIFT                          0

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* RFM_GRB :: CTRL :: reserved0 [31:01] */
#define BCHP_RFM_GRB_CTRL_reserved0_MASK                           0xfffffffe
#define BCHP_RFM_GRB_CTRL_reserved0_SHIFT                          1

/* RFM_GRB :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_MASK                     0x00000001
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_SHIFT                    0
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_INTR_DISABLE             0
#define BCHP_RFM_GRB_CTRL_gisb_error_intr_INTR_ENABLE              1

/***************************************************************************
 *SW_RESET_0 - GR Bridge Software Reset 0 Register
 ***************************************************************************/
/* RFM_GRB :: SW_RESET_0 :: reserved0 [31:00] */
#define BCHP_RFM_GRB_SW_RESET_0_reserved0_MASK                     0xffffffff
#define BCHP_RFM_GRB_SW_RESET_0_reserved0_SHIFT                    0

/***************************************************************************
 *SW_RESET_1 - GR Bridge Software Reset 1 Register
 ***************************************************************************/
/* RFM_GRB :: SW_RESET_1 :: reserved0 [31:00] */
#define BCHP_RFM_GRB_SW_RESET_1_reserved0_MASK                     0xffffffff
#define BCHP_RFM_GRB_SW_RESET_1_reserved0_SHIFT                    0

#endif /* #ifndef BCHP_RFM_GRB_H__ */

/* End of File */