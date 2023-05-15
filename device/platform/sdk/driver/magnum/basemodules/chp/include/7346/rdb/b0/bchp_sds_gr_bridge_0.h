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
 * $brcm_Workfile: bchp_sds_gr_bridge_0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/12/11 3:37p $
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
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_sds_gr_bridge_0.h $
 * 
 * Hydra_Software_Devel/2   12/12/11 3:37p pntruong
 * SW7346-143: Resynced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_GR_BRIDGE_0_H__
#define BCHP_SDS_GR_BRIDGE_0_H__

/***************************************************************************
 *SDS_GR_BRIDGE_0 - SDS GISB Bridge Registers 0
 ***************************************************************************/
#define BCHP_SDS_GR_BRIDGE_0_REVISION            0x00700c00 /* GR Bridge Revision */
#define BCHP_SDS_GR_BRIDGE_0_CTRL                0x00700c04 /* GR Bridge Control Register */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0           0x00700c08 /* GR Bridge Software Init 0 Register */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1           0x00700c0c /* GR Bridge Software Init 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* SDS_GR_BRIDGE_0 :: REVISION :: reserved0 [31:16] */
#define BCHP_SDS_GR_BRIDGE_0_REVISION_reserved0_MASK               0xffff0000
#define BCHP_SDS_GR_BRIDGE_0_REVISION_reserved0_SHIFT              16

/* SDS_GR_BRIDGE_0 :: REVISION :: MAJOR [15:08] */
#define BCHP_SDS_GR_BRIDGE_0_REVISION_MAJOR_MASK                   0x0000ff00
#define BCHP_SDS_GR_BRIDGE_0_REVISION_MAJOR_SHIFT                  8
#define BCHP_SDS_GR_BRIDGE_0_REVISION_MAJOR_DEFAULT                0x00000001

/* SDS_GR_BRIDGE_0 :: REVISION :: MINOR [07:00] */
#define BCHP_SDS_GR_BRIDGE_0_REVISION_MINOR_MASK                   0x000000ff
#define BCHP_SDS_GR_BRIDGE_0_REVISION_MINOR_SHIFT                  0
#define BCHP_SDS_GR_BRIDGE_0_REVISION_MINOR_DEFAULT                0x00000000

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* SDS_GR_BRIDGE_0 :: CTRL :: reserved0 [31:01] */
#define BCHP_SDS_GR_BRIDGE_0_CTRL_reserved0_MASK                   0xfffffffe
#define BCHP_SDS_GR_BRIDGE_0_CTRL_reserved0_SHIFT                  1

/* SDS_GR_BRIDGE_0 :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_SDS_GR_BRIDGE_0_CTRL_gisb_error_intr_MASK             0x00000001
#define BCHP_SDS_GR_BRIDGE_0_CTRL_gisb_error_intr_SHIFT            0
#define BCHP_SDS_GR_BRIDGE_0_CTRL_gisb_error_intr_DEFAULT          0x00000000
#define BCHP_SDS_GR_BRIDGE_0_CTRL_gisb_error_intr_INTR_DISABLE     0
#define BCHP_SDS_GR_BRIDGE_0_CTRL_gisb_error_intr_INTR_ENABLE      1

/***************************************************************************
 *SW_INIT_0 - GR Bridge Software Init 0 Register
 ***************************************************************************/
/* SDS_GR_BRIDGE_0 :: SW_INIT_0 :: reserved0 [31:03] */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_reserved0_MASK              0xfffffff8
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_reserved0_SHIFT             3

/* SDS_GR_BRIDGE_0 :: SW_INIT_0 :: TUNER_SW_INIT [02:02] */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_TUNER_SW_INIT_MASK          0x00000004
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_TUNER_SW_INIT_SHIFT         2
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_TUNER_SW_INIT_DEFAULT       0x00000000
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_TUNER_SW_INIT_DEASSERT      0
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_TUNER_SW_INIT_ASSERT        1

/* SDS_GR_BRIDGE_0 :: SW_INIT_0 :: DSEC_SW_INIT [01:01] */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_DSEC_SW_INIT_MASK           0x00000002
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_DSEC_SW_INIT_SHIFT          1
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_DSEC_SW_INIT_DEFAULT        0x00000000
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_DSEC_SW_INIT_DEASSERT       0
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_DSEC_SW_INIT_ASSERT         1

/* SDS_GR_BRIDGE_0 :: SW_INIT_0 :: SDS_SW_INIT [00:00] */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_SDS_SW_INIT_MASK            0x00000001
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_SDS_SW_INIT_SHIFT           0
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_SDS_SW_INIT_DEFAULT         0x00000000
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_SDS_SW_INIT_DEASSERT        0
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_0_SDS_SW_INIT_ASSERT          1

/***************************************************************************
 *SW_INIT_1 - GR Bridge Software Init 1 Register
 ***************************************************************************/
/* SDS_GR_BRIDGE_0 :: SW_INIT_1 :: reserved0 [31:01] */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_reserved0_MASK              0xfffffffe
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_reserved0_SHIFT             1

/* SDS_GR_BRIDGE_0 :: SW_INIT_1 :: SPARE_SW_INIT [00:00] */
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_MASK          0x00000001
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_SHIFT         0
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_DEFAULT       0x00000001
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_DEASSERT      0
#define BCHP_SDS_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_ASSERT        1

#endif /* #ifndef BCHP_SDS_GR_BRIDGE_0_H__ */

/* End of File */