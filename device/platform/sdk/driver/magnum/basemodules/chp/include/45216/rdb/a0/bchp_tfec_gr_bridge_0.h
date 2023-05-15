/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Thu Feb 20 15:33:52 2014
 *                 Full Compile MD5 Checksum 1766fea499add5f6ee91330ef96d35c5
 *                   (minus title and desc)
 *                 MD5 Checksum              4c358fb5b94802f03aec82d8df2c9afa
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 *
 ***************************************************************************/



/****************************************************************************
 ***************************************************************************/

#ifndef BCHP_TFEC_GR_BRIDGE_0_H__
#define BCHP_TFEC_GR_BRIDGE_0_H__

/***************************************************************************
 *TFEC_GR_BRIDGE_0 - TFEC GISB Bridge Registers
 ***************************************************************************/
#define BCHP_TFEC_GR_BRIDGE_0_REVISION           0x05000300 /* GR Bridge Revision */
#define BCHP_TFEC_GR_BRIDGE_0_CTRL               0x05000304 /* GR Bridge Control Register */
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0          0x05000308 /* GR Bridge Software Init 0 Register */
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1          0x0500030c /* GR Bridge Software Init 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* TFEC_GR_BRIDGE_0 :: REVISION :: reserved0 [31:16] */
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_reserved0_MASK              0xffff0000
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_reserved0_SHIFT             16

/* TFEC_GR_BRIDGE_0 :: REVISION :: MAJOR [15:08] */
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_MAJOR_MASK                  0x0000ff00
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_MAJOR_SHIFT                 8
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_MAJOR_DEFAULT               0x00000001

/* TFEC_GR_BRIDGE_0 :: REVISION :: MINOR [07:00] */
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_MINOR_MASK                  0x000000ff
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_MINOR_SHIFT                 0
#define BCHP_TFEC_GR_BRIDGE_0_REVISION_MINOR_DEFAULT               0x00000000

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* TFEC_GR_BRIDGE_0 :: CTRL :: reserved0 [31:01] */
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_reserved0_MASK                  0xfffffffe
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_reserved0_SHIFT                 1

/* TFEC_GR_BRIDGE_0 :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_gisb_error_intr_MASK            0x00000001
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_gisb_error_intr_SHIFT           0
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_gisb_error_intr_DEFAULT         0x00000000
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_gisb_error_intr_INTR_DISABLE    0
#define BCHP_TFEC_GR_BRIDGE_0_CTRL_gisb_error_intr_INTR_ENABLE     1

/***************************************************************************
 *SW_INIT_0 - GR Bridge Software Init 0 Register
 ***************************************************************************/
/* TFEC_GR_BRIDGE_0 :: SW_INIT_0 :: reserved0 [31:01] */
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_reserved0_MASK             0xfffffffe
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_reserved0_SHIFT            1

/* TFEC_GR_BRIDGE_0 :: SW_INIT_0 :: TFEC_SW_INIT [00:00] */
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_TFEC_SW_INIT_MASK          0x00000001
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_TFEC_SW_INIT_SHIFT         0
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_TFEC_SW_INIT_DEFAULT       0x00000000
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_TFEC_SW_INIT_DEASSERT      0
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_0_TFEC_SW_INIT_ASSERT        1

/***************************************************************************
 *SW_INIT_1 - GR Bridge Software Init 1 Register
 ***************************************************************************/
/* TFEC_GR_BRIDGE_0 :: SW_INIT_1 :: reserved0 [31:01] */
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_reserved0_MASK             0xfffffffe
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_reserved0_SHIFT            1

/* TFEC_GR_BRIDGE_0 :: SW_INIT_1 :: SPARE_SW_INIT [00:00] */
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_MASK         0x00000001
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_SHIFT        0
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_DEFAULT      0x00000001
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_DEASSERT     0
#define BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1_SPARE_SW_INIT_ASSERT       1

#endif /* #ifndef BCHP_TFEC_GR_BRIDGE_0_H__ */

/* End of File */