/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_moca_gr_bridge.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 9:03p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:24:52 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_moca_gr_bridge.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 9:03p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_MOCA_GR_BRIDGE_H__
#define BCHP_MOCA_GR_BRIDGE_H__

/***************************************************************************
 *MOCA_GR_BRIDGE - Registers for the MoCA block's GR Bridge
 ***************************************************************************/
#define BCHP_MOCA_GR_BRIDGE_REVISION             0x002a20c0 /* GR Bridge Revision */
#define BCHP_MOCA_GR_BRIDGE_CTRL                 0x002a20c4 /* GR Bridge Control Register */
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0           0x002a20c8 /* GR Bridge Software Reset 0 Register */
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1           0x002a20cc /* GR Bridge Software Reset 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* MOCA_GR_BRIDGE :: REVISION :: reserved0 [31:16] */
#define BCHP_MOCA_GR_BRIDGE_REVISION_reserved0_MASK                0xffff0000
#define BCHP_MOCA_GR_BRIDGE_REVISION_reserved0_SHIFT               16

/* MOCA_GR_BRIDGE :: REVISION :: MAJOR [15:08] */
#define BCHP_MOCA_GR_BRIDGE_REVISION_MAJOR_MASK                    0x0000ff00
#define BCHP_MOCA_GR_BRIDGE_REVISION_MAJOR_SHIFT                   8

/* MOCA_GR_BRIDGE :: REVISION :: MINOR [07:00] */
#define BCHP_MOCA_GR_BRIDGE_REVISION_MINOR_MASK                    0x000000ff
#define BCHP_MOCA_GR_BRIDGE_REVISION_MINOR_SHIFT                   0

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* MOCA_GR_BRIDGE :: CTRL :: reserved0 [31:01] */
#define BCHP_MOCA_GR_BRIDGE_CTRL_reserved0_MASK                    0xfffffffe
#define BCHP_MOCA_GR_BRIDGE_CTRL_reserved0_SHIFT                   1

/* MOCA_GR_BRIDGE :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_MOCA_GR_BRIDGE_CTRL_gisb_error_intr_MASK              0x00000001
#define BCHP_MOCA_GR_BRIDGE_CTRL_gisb_error_intr_SHIFT             0
#define BCHP_MOCA_GR_BRIDGE_CTRL_gisb_error_intr_INTR_DISABLE      0
#define BCHP_MOCA_GR_BRIDGE_CTRL_gisb_error_intr_INTR_ENABLE       1

/***************************************************************************
 *SW_RESET_0 - GR Bridge Software Reset 0 Register
 ***************************************************************************/
/* MOCA_GR_BRIDGE :: SW_RESET_0 :: reserved0 [31:01] */
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0_reserved0_MASK              0xfffffffe
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0_reserved0_SHIFT             1

/* MOCA_GR_BRIDGE :: SW_RESET_0 :: SPARE_SW_RESET [00:00] */
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0_SPARE_SW_RESET_MASK         0x00000001
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0_SPARE_SW_RESET_SHIFT        0
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0_SPARE_SW_RESET_DEASSERT     0
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_0_SPARE_SW_RESET_ASSERT       1

/***************************************************************************
 *SW_RESET_1 - GR Bridge Software Reset 1 Register
 ***************************************************************************/
/* MOCA_GR_BRIDGE :: SW_RESET_1 :: reserved0 [31:01] */
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1_reserved0_MASK              0xfffffffe
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1_reserved0_SHIFT             1

/* MOCA_GR_BRIDGE :: SW_RESET_1 :: SPARE_SW_RESET [00:00] */
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1_SPARE_SW_RESET_MASK         0x00000001
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1_SPARE_SW_RESET_SHIFT        0
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1_SPARE_SW_RESET_DEASSERT     0
#define BCHP_MOCA_GR_BRIDGE_SW_RESET_1_SPARE_SW_RESET_ASSERT       1

#endif /* #ifndef BCHP_MOCA_GR_BRIDGE_H__ */

/* End of File */